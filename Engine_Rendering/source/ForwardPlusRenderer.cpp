#include "ForwardPlusRenderer.h"
#include "GraphicsContext.h"
#include "MaterialManager.h"
#include "RenderState.h"
#include "Camera.h"
#include "ConstantBufferTypes.h"
#include "LightPass.h"
#include "ShadowMapping.h"
#include "HighDynamicRange.h"
#include "ScreenSpaceAmbientOcclusion.h"
#include "Profiler.h"

static FConfigVariable ShadowMappingEnabled("Forward+", "Shadow Mapping Enabled", true);
static FConfigVariable SSAOEnabled("Forward+", "SSAO Enabled", true);
static FConfigVariable ShouldRecompileMaterials("Forward+", "Should Recompile Material", false);
static FConfigVariable ForceWireframe("Forward+", "Force Wireframe", false);
static FConfigVariable ShowHdrDebug("Forward+", "Show Hdr Debug", false);

ConstantString FForwardPlusRenderer::kDepthNormalMaterial = "DepthNormal";
ConstantString FForwardPlusRenderer::kDepthNormalBumpMaterial = "DepthNormalBump";

FForwardPlusRenderer::FForwardPlusRenderer(class FGraphicsContext* GraphicsContext, class FMaterialManager* MaterialManager) : GraphicsContext(GraphicsContext), MaterialManager(MaterialManager)
{
	MaterialManager->RecompileMaterials(GenerateGlobalDefines());

	LightPass = Make_Unique<FLightPass>(GraphicsContext, MaterialManager);
	LightPass->Initialise();

	ShadowMapping = Make_Unique<FShadowMapping>(GraphicsContext, MaterialManager);
	ShadowMapping->Initialise();

	HighDynamicRange = Make_Unique<FHighDynamicRange>(GraphicsContext, MaterialManager);
	HighDynamicRange->Initialise();

	AmbientOcclusion = Make_Unique<FScreenSpaceAmbientOcclusion>(GraphicsContext, MaterialManager);
	AmbientOcclusion->Initialise();

	HighDynamicRangeMSAATarget = Make_Unique<FRenderTarget>(GraphicsContext, ERenderTargetType::Color | ERenderTargetType::Depth, GraphicsContext->GetWidth(), GraphicsContext->GetHeight(), DXGI_FORMAT_R32G32B32A32_FLOAT, DXGI_FORMAT_R32G32B32A32_FLOAT, DXGI_FORMAT_D24_UNORM_S8_UINT, false, 4);
	HighDynamicRangeTarget = Make_Unique<FRenderTarget>(GraphicsContext, ERenderTargetType::Color, GraphicsContext->GetWidth(), GraphicsContext->GetHeight(), DXGI_FORMAT_R32G32B32A32_FLOAT, DXGI_FORMAT_R32G32B32A32_FLOAT);
	OcclusionTarget = Make_Unique<FRenderTarget>(GraphicsContext, ERenderTargetType::Color, GraphicsContext->GetWidth(), GraphicsContext->GetHeight(), DXGI_FORMAT_R8_UNORM, DXGI_FORMAT_R8_UNORM);
	NormalTarget = Make_Unique<FRenderTarget>(GraphicsContext, ERenderTargetType::Color, GraphicsContext->GetWidth(), GraphicsContext->GetHeight(), DXGI_FORMAT_R32G32B32A32_FLOAT, DXGI_FORMAT_R32G32B32A32_FLOAT);
	DepthTarget = Make_Unique<FRenderTarget>(GraphicsContext, ERenderTargetType::Color, GraphicsContext->GetWidth(), GraphicsContext->GetHeight(), DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_R32_FLOAT);

	InstanceBuffer = Make_Unique<FInstanceBuffer>();
	DepthNormalMaterial = MaterialManager->GetMaterial(kDepthNormalMaterial);
	DepthNormalBumpMaterial = MaterialManager->GetMaterial(kDepthNormalBumpMaterial);

#if DEBUG
	SpriteBatch = Make_Unique<FSpriteBatch>(GraphicsContext, MaterialManager->GetPointSpriteMaterial(), 4);
#endif
}

FForwardPlusRenderer::~FForwardPlusRenderer()
{

}

void FForwardPlusRenderer::Update(float DeltaTime)
{
	LastDeltaTime = DeltaTime;
	TotalTime += DeltaTime;

	if (ShouldRecompileMaterials.AsBoolean())
	{
		RecompileMaterials();
		ShouldRecompileMaterials.AsBoolean() = false;
	}
}

void FForwardPlusRenderer::DrawScene(const FCamera& Camera, class IScene* Scene)
{
	FProfiler::Get().PushSection("Rendering", "Forward+");

	FMaterialParameters::FlushShaderResources(GraphicsContext->GetImmediateContext());

	CurrentViewport = Camera.GetViewport();

	// Setup buffers
	UpdateFrameData(Camera, Scene);

	{
		FProfiler::Get().PushSection("Rendering", "Depth-Normal PrePass");
		// We use the result of this pass for both SSAO and Light Culling (we need depth information for that).
		DoDepthNormalPrePass(Camera, Scene);
		if (SSAOEnabled.AsBoolean())
		{
			AmbientOcclusion->Apply(CurrentViewport, DepthTarget.Get(), NormalTarget.Get(), OcclusionTarget.Get());
			MaterialManager->SetGlobalResourceView(FScreenSpaceAmbientOcclusion::kAmbientOcclusionMap, OcclusionTarget->GetRenderResource());
		}
		FProfiler::Get().PopSection();
	}

	{
		FProfiler::Get().PushSection("Rendering", "Shadow Maps Pass");
		// Render shadow maps
		if (ShadowMappingEnabled.AsBoolean())
			ShadowMapping->GatherShadowMaps(Scene);
		FProfiler::Get().PopSection();
	}

	{
		FProfiler::Get().PushSection("Rendering", "Light Culling Pass");
		// Perform light culling and upload light buffers
		LightPass->Apply(this, ShadowMapping.Get(), DepthTarget.Get(), Scene, Camera);
		FProfiler::Get().PopSection();
	}

	{
		FProfiler::Get().PushSection("Rendering", "Scene Pass");
		// Render the scene in forward fashion
		DoScenePass(Camera, Scene);
		FProfiler::Get().PopSection();
	}

	{
		FProfiler::Get().PushSection("Rendering", "HDR Post Process Pass");
		HighDynamicRangeTarget->ResolveFrom(GraphicsContext->GetImmediateContext(), HighDynamicRangeMSAATarget.Get());
		HighDynamicRange->Apply(CurrentViewport, HighDynamicRangeTarget.Get(), GraphicsContext->GetBackBuffer());
		FProfiler::Get().PopSection();
	}

	FProfiler::Get().PopSection();
#if DEBUG
	FViewport Fullscreen(0, 0, GraphicsContext->GetWidth(), GraphicsContext->GetHeight());
	D3D11_VIEWPORT Viewport = Fullscreen.CreateRenderViewport();
	ID3D11RenderTargetView* RenderTarget = GraphicsContext->GetBackBuffer();
	GraphicsContext->GetImmediateContext()->RSSetViewports(1, &Viewport);
	GraphicsContext->GetImmediateContext()->ClearDepthStencilView(GraphicsContext->GetDepthStencil(), D3D11_CLEAR_STENCIL | D3D11_CLEAR_DEPTH, 1.0f, 0);
	GraphicsContext->GetImmediateContext()->OMSetRenderTargets(1, &RenderTarget, GraphicsContext->GetDepthStencil());
	if (ShowHdrDebug.AsBoolean())
	{
		HighDynamicRange->DebugHdr(SpriteBatch.Get());
		FInstanceBuffer InstanceBuffer = FInstanceBuffer::Identity;
		FDrawCall DrawCall(GraphicsContext->GetImmediateContext(), &InstanceBuffer, nullptr);
		SpriteBatch->Render(DrawCall);
	}
#endif
}

void FForwardPlusRenderer::RecompileMaterials()
{
	DefinesVector Defines = GenerateGlobalDefines();
	MaterialManager->RecompileMaterials(Defines);
	AmbientOcclusion->Initialise();
	HighDynamicRange->UpdateSettings();
}

void FForwardPlusRenderer::UpdateFrameData(const FCamera& Camera, class IScene* Scene)
{
	FRenderFrameData FrameData;

	FrameData.FrameTime = TotalTime;
	FrameData.DeltaTime = LastDeltaTime;
	FrameData.ProjectionMatrix = Camera.GetProjectionMatrix();
	FrameData.ViewMatrix = Camera.GetViewMatrix();
	FrameData.CameraPosition = Camera.GetPosition();
	FrameData.SpotLightCount = Scene->GetSpotLights().Size();
	FrameData.PointLightCount = Scene->GetPointLights().Size();

	MaterialManager->UpdateMaterialBuffers(FrameData, GraphicsContext);
}

DefinesVector FForwardPlusRenderer::GenerateGlobalDefines()
{
	DefinesVector Defines;

	if (ShadowMappingEnabled.AsBoolean())
	{
		Defines.Add("SHADOW_MAPPING");
	}

	if (SSAOEnabled.AsBoolean())
	{
		Defines.Add("SSAO_ENABLED");
	}

	return Defines;
}

void FForwardPlusRenderer::DoScenePass(const FCamera& Camera, class IScene* Scene)
{
	// Clear depth buffer
	ID3D11DeviceContext* DeviceContext = GraphicsContext->GetImmediateContext();

	const FLOAT ClearColour[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	ID3D11RenderTargetView* BackBuffer = HighDynamicRangeMSAATarget->GetRenderTarget();
	DeviceContext->ClearDepthStencilView(HighDynamicRangeMSAATarget->GetDepthStencil(), D3D11_CLEAR_STENCIL | D3D11_CLEAR_DEPTH, 1.0f, 0);
	DeviceContext->ClearRenderTargetView(BackBuffer, ClearColour);

	DeviceContext->OMSetRenderTargets(1, &BackBuffer, HighDynamicRangeMSAATarget->GetDepthStencil());

	FViewport Fullscreen(0, 0, GraphicsContext->GetWidth(), GraphicsContext->GetHeight());
	D3D11_VIEWPORT Viewport = Fullscreen.CreateRenderViewport();
	DeviceContext->RSSetViewports(1, &Viewport);

	InstanceBuffer->SetWorld(glm::mat4(1.0));

	ID3D11RasterizerState* PreviousState = nullptr;

	if (ForceWireframe.AsBoolean())
	{
		DeviceContext->RSGetState(&PreviousState);
		DeviceContext->RSSetState(GraphicsContext->GetRenderStates()->WireframeRasterizer);
	}

	FDrawCall Params(DeviceContext, InstanceBuffer.Get());
	Scene->Draw(Params);

	if (PreviousState != nullptr)
	{
		DeviceContext->RSSetState(PreviousState);
	}
}

void FForwardPlusRenderer::DoDepthNormalPrePass(const FCamera& Camera, class IScene* Scene)
{
	// Clear depth buffer
	ID3D11DeviceContext* DeviceContext = GraphicsContext->GetImmediateContext();

	const FLOAT NormalsClearColour[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	const FLOAT DepthClearColour[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	ID3D11RenderTargetView* Targets[2] = { NormalTarget->GetRenderTarget(), DepthTarget->GetRenderTarget() };
	DeviceContext->ClearDepthStencilView(GraphicsContext->GetDepthStencil(), D3D11_CLEAR_STENCIL | D3D11_CLEAR_DEPTH, 1.0f, 0);

	DeviceContext->ClearRenderTargetView(NormalTarget->GetRenderTarget(), NormalsClearColour);
	DeviceContext->ClearRenderTargetView(DepthTarget->GetRenderTarget(), DepthClearColour);

	DeviceContext->OMSetRenderTargets(2, Targets, GraphicsContext->GetDepthStencil());

	FViewport Fullscreen(0, 0, GraphicsContext->GetWidth(), GraphicsContext->GetHeight());
	D3D11_VIEWPORT Viewport = Fullscreen.CreateRenderViewport();
	DeviceContext->RSSetViewports(1, &Viewport);

	InstanceBuffer->SetWorld(glm::mat4(1.0));
	FDrawCall Params(DeviceContext, InstanceBuffer.Get());
	Params.OverrideMaterial = std::bind(&FForwardPlusRenderer::OverrideMaterialDepthNormalPass, this, std::placeholders::_1);
	Scene->Draw(Params);
}

FMaterial* FForwardPlusRenderer::OverrideMaterialDepthNormalPass(FMaterial* CurrentMaterial)
{
	if (CurrentMaterial->GetParameters()->HasResource(FTextureNames::kNormalMap))
	{
		return DepthNormalBumpMaterial;
	}

	return DepthNormalMaterial;
}