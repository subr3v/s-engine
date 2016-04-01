#include "CoreTypes.h"
#include "LightPass.h"
#include "RenderTarget.h"
#include "ConstantBufferTypes.h"
#include "GraphicsContext.h"
#include "ShadowMapping.h"
#include "MemoryUtils.h"
#include "MaterialParameters.h"
#include "Material.h"
#include "MaterialManager.h"

// These constants have to match value with the constants defined in tiled_lighting.hlsl
static const u32 kMaxPointLightNum = 4096;
static const u32 kMaxLightPerTile = 128;
static const u32 kMaxSpotLightNum = 4096;
static const u32 kTileSize = 16;

static const ConstantString kPointLightGeometryBufferName = "PointLightGeometryData";
static const ConstantString kPointLightParameterBufferName = "PointLightParameterData";
static const ConstantString kSpotLightGeometryBufferName = "SpotLightGeometryData";
static const ConstantString kSpotLightParameterBufferName = "SpotLightParameterData";
static const ConstantString kTileToLightIndexMap = "TileToIndexMap";
static const ConstantString kLightBufferName = "LightBuffer";

FLightPass::FLightPass(FGraphicsContext* GraphicsContext, FMaterialManager* MaterialManager) : GraphicsContext(GraphicsContext), MaterialManager(MaterialManager)
{
	LightBuffer = Make_Unique<FLightBuffer>();
}

FLightPass::~FLightPass()
{

}

static u32 GetNumTilesX(u32 Width)
{
	return (u32)((Width + kTileSize - 1) / (float)kTileSize);
}

static u32 GetNumTilesY(u32 Height)
{
	return (u32)((Height + kTileSize - 1) / (float)kTileSize);
}

void FLightPass::Initialise()
{
	SizeType LightBufferSizeInBytes = sizeof(glm::vec4) * kMaxPointLightNum;
	SizeType SpotLightParameterBufferSizeInBytes = sizeof(glm::vec4) * kMaxPointLightNum * 3;
	SizeType NumTiles = GetNumTilesX(GraphicsContext->GetWidth()) * GetNumTilesY(GraphicsContext->GetHeight());
	SizeType TileMapBufferSizeInBytes = (sizeof(u32) * kMaxLightPerTile) * NumTiles;

	EBufferType BufferType = EBufferType::ShaderResource;
	EBufferType IndexMapBufferType = (EBufferType)(EBufferType::ShaderResource | EBufferType::UnorderedAccess);
	PointLightGeometryBuffer = Make_Unique<FGraphicsBuffer>(GraphicsContext, BufferType, EBufferUsage::Dynamic, LightBufferSizeInBytes, nullptr);
	PointLightParameterBuffer = Make_Unique<FGraphicsBuffer>(GraphicsContext, BufferType, EBufferUsage::Dynamic, LightBufferSizeInBytes, nullptr);
	SpotLightGeometryBuffer = Make_Unique<FGraphicsBuffer>(GraphicsContext, BufferType, EBufferUsage::Dynamic, LightBufferSizeInBytes, nullptr);
	SpotLightParameterBuffer = Make_Unique<FGraphicsBuffer>(GraphicsContext, BufferType, EBufferUsage::Dynamic, SpotLightParameterBufferSizeInBytes, nullptr);
	TileToLightIndexMap = Make_Unique<FGraphicsBuffer>(GraphicsContext, IndexMapBufferType, EBufferUsage::Default, TileMapBufferSizeInBytes, nullptr);

	PointLightGeometryBuffer->CreateResourceView(GraphicsContext, DXGI_FORMAT_R32G32B32A32_FLOAT, kMaxPointLightNum);
	PointLightParameterBuffer->CreateResourceView(GraphicsContext, DXGI_FORMAT_R32G32B32A32_FLOAT, kMaxPointLightNum);
	SpotLightGeometryBuffer->CreateResourceView(GraphicsContext, DXGI_FORMAT_R32G32B32A32_FLOAT, kMaxPointLightNum);
	SpotLightParameterBuffer->CreateResourceView(GraphicsContext, DXGI_FORMAT_R32G32B32A32_FLOAT, kMaxPointLightNum);
	TileToLightIndexMap->CreateResourceView(GraphicsContext, DXGI_FORMAT_R32_UINT, NumTiles * kMaxLightPerTile);
	TileToLightIndexMap->CreateUnorderedAccessView(GraphicsContext, DXGI_FORMAT_R32_UINT, NumTiles * kMaxLightPerTile);

	FShaderProgramInfo LightShaderInfo(L"Shaders\\light_culling.hlsl", "mainCS", "cs_5_0");
	LightCullingShader = Make_Unique<FComputeShader>(GraphicsContext, &LightShaderInfo);
}

void FLightPass::Apply(FForwardPlusRenderer* Renderer, FShadowMapping* ShadowMap, FRenderTarget* DepthTarget, IScene* Scene, const FCamera& Camera)
{
	UploadBuffers(Scene, Camera, ShadowMap, DepthTarget);

	if (ShadowMap != nullptr)
	{
		MaterialManager->SetGlobalResourceView(FShadowMapping::kShadowMap, ShadowMap->GetShadowMap());
	}
}

void FLightPass::UploadBuffers(IScene* Scene, const FCamera& Camera, FShadowMapping* ShadowMap, FRenderTarget* DepthTarget)
{
	ID3D11DeviceContext* DeviceContext = GraphicsContext->GetImmediateContext();

	auto AmbientLight = Scene->GetAmbientLight();
	auto SpotLights = Scene->GetSpotLights();
	auto PointLights = Scene->GetPointLights();
	auto DirectionalLights = Scene->GetDirectionalLights();

	typedef glm::vec4 float4;

	float4* PointLightGeometryData = (float4*)PointLightGeometryBuffer->MapData(DeviceContext);
	float4* PointLightParametersData = (float4*)PointLightParameterBuffer->MapData(DeviceContext);

	for (size_t i = 0; i < PointLights.Size(); i++)
	{
		FPointLight& PointLight = PointLights[i];
		float4& GeometryData = (*PointLightGeometryData++);
		float4& ParameterData = (*PointLightParametersData++);

		// xyz hold position and w holds radius
		GeometryData.xyz = PointLight.Position;
		GeometryData.w = PointLight.Radius;

		// Parameters xyz holds colour and parameter w holds intensity.
		ParameterData.xyz = PointLight.Colour;
		ParameterData.w = PointLight.Intensity;
	}

	PointLightGeometryBuffer->UnMapData(DeviceContext);
	PointLightParameterBuffer->UnMapData(DeviceContext);

	float4* SpotLightGeometryData = (float4*)SpotLightGeometryBuffer->MapData(DeviceContext);
	float4* SpotLightParametersData = (float4*)SpotLightParameterBuffer->MapData(DeviceContext);

	s32 CurrentShadowIndex = 0;
	for (size_t i = 0; i < SpotLights.Size(); i++)
	{
		FSpotlight& SpotLight = SpotLights[i];
		float4& GeometryData = (*SpotLightGeometryData++);
		float4& ParameterData1 = (*SpotLightParametersData++);
		float4& ParameterData2 = (*SpotLightParametersData++);
		float4& ParameterData3 = (*SpotLightParametersData++);

		// xyz hold position and w holds radius
		GeometryData.xyz = SpotLight.Position;
		GeometryData.w = SpotLight.Radius;

		// Parameters1 xyz holds colour and parameter w holds intensity.
		ParameterData1.xyz = SpotLight.Colour;
		ParameterData1.w = SpotLight.Intensity;
		
		// Parameters2 xyz holds spotlight direction and angle
		ParameterData2.xyz = SpotLight.Direction;
		ParameterData2.w = glm::radians(SpotLight.Phi);

		// Parameters3 holds shadow informations (cast shadows flag and shadow index)
		ParameterData3.x = SpotLight.bCastShadow;
		if (SpotLight.bCastShadow)
		{
			ParameterData3.y = (float)(CurrentShadowIndex++);
		}
	}

	SpotLightGeometryBuffer->UnMapData(DeviceContext);
	SpotLightParameterBuffer->UnMapData(DeviceContext);

	// TODO: Sort out ambient light data.

	LightCullingShader->Bind(DeviceContext);

	// Shaders resources are:
	// t0 - Point Geometry Data
	// t1 - Spot Geometry Data
	// t2 - Depth buffer
	// u0 - IndexTileMap

	ID3D11Buffer* FrameDataBuffer = MaterialManager->GetDiffuseMaterial()->GetParameters()->GetUniformBuffer(FMaterialManager::kFrameDataBufferName)->GetBufferHandle()->GetBuffer();

	ID3D11ShaderResourceView* ResourceViews[3] = { PointLightGeometryBuffer->GetResourceView(), SpotLightGeometryBuffer->GetResourceView(), DepthTarget->GetRenderResource() };
	ID3D11UnorderedAccessView* UnorderedAccessViews[1] = { TileToLightIndexMap->GetUnorderdAccessView() };
	ID3D11ShaderResourceView* NullResourceViews[3] = { nullptr, nullptr, nullptr };
	ID3D11UnorderedAccessView* NullUnorderedAccessViews[1] = { nullptr };
	UINT InitialsCount[1] = { 0 };

	LightCullingShader->SetShaderResources(DeviceContext, ResourceViews, 3);
	LightCullingShader->SetShaderUnorderedAccessViews(DeviceContext, UnorderedAccessViews, 1, InitialsCount);
	LightCullingShader->SetShaderConstantBuffers(DeviceContext, &FrameDataBuffer, 1);
	LightCullingShader->Execute(DeviceContext, GetNumTilesX(GraphicsContext->GetWidth()), GetNumTilesY(GraphicsContext->GetHeight()), 1);
	LightCullingShader->SetShaderResources(DeviceContext, NullResourceViews, 3);
	LightCullingShader->SetShaderUnorderedAccessViews(DeviceContext, NullUnorderedAccessViews, 1, InitialsCount);

	// Set parameters for lighting shaders
	MaterialManager->SetGlobalResourceView(kPointLightGeometryBufferName, PointLightGeometryBuffer->GetResourceView());
	MaterialManager->SetGlobalResourceView(kPointLightParameterBufferName, PointLightParameterBuffer->GetResourceView());
	MaterialManager->SetGlobalResourceView(kSpotLightGeometryBufferName, SpotLightGeometryBuffer->GetResourceView());
	MaterialManager->SetGlobalResourceView(kSpotLightParameterBufferName, SpotLightParameterBuffer->GetResourceView());
	MaterialManager->SetGlobalResourceView(kTileToLightIndexMap, TileToLightIndexMap->GetResourceView());

	if (ShadowMap != nullptr)
	{
		ShadowMap->UploadShadowBuffer(DeviceContext);
	}

	LightBuffer->AmbientLight = glm::vec4(AmbientLight.Colour, 1.0f) * AmbientLight.Intensity;
	MaterialManager->UploadGlobalUniformBuffer(kLightBufferName, LightBuffer.Get(), sizeof(FLightBuffer));
}
