#include "ShadowMapping.h"
#include <d3d11.h>
#include "Camera.h"
#include "DrawCallParameters.h"
#include "ConstantBufferTypes.h"
#include "GraphicsContext.h"
#include "MaterialManager.h"
#include "BaseScene.h"
#include "glm/gtc/matrix_transform.hpp"

static const DXGI_FORMAT ShadowMapTextureFormat = DXGI_FORMAT_R32_FLOAT;
static const DXGI_FORMAT ShadowMapDepthFormat = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;

ConstantString FShadowBuffer::kBufferName = "ShadowBuffer";
ConstantString FShadowInstanceBuffer::kBufferName = "ShadowInstanceBuffer";
ConstantString FShadowMapping::kShadowMap = "ShadowTexture";

FShadowMapping::FShadowMapping(FGraphicsContext* GraphicsContext, FMaterialManager* MaterialManager) : GraphicsContext(GraphicsContext), MaterialManager(MaterialManager)
{
	ShadowBuffer = Make_Unique<FShadowBuffer>();
}

FShadowMapping::~FShadowMapping()
{
}

void FShadowMapping::Initialise()
{
	DepthOnlyMaterial = MaterialManager->GetDepthMaterial();
	ShadowMap = Make_Unique<FRenderTarget>(GraphicsContext, ERenderTargetType::Color | ERenderTargetType::Depth, ShadowMapResolution.x, ShadowMapResolution.y, ShadowMapTextureFormat, ShadowMapTextureFormat, ShadowMapDepthFormat);
}

void FShadowMapping::GatherShadowMaps(class IScene* Scene)
{
	ID3D11DeviceContext* DeviceContext = GraphicsContext->GetImmediateContext();

	ID3D11RenderTargetView* RenderTarget = ShadowMap->GetRenderTarget();
	const FLOAT ClearColour[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	DeviceContext->ClearRenderTargetView(RenderTarget, ClearColour);
	DeviceContext->ClearDepthStencilView(ShadowMap->GetDepthStencil(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	DeviceContext->OMSetRenderTargets(1, &RenderTarget, ShadowMap->GetDepthStencil());

	UpdateShadowBuffer(Scene);

	int CurrentIndex = 0;
	for (auto SpotLight : Scene->GetSpotLights())
	{
		if (SpotLight.bCastShadow)
		{
			RenderShadowMap(Scene, ShadowMap.Get(), CurrentIndex);
			CurrentIndex++;
		}
	}
}

void FShadowMapping::RenderShadowMap(IScene* Scene, FRenderTarget* Target, int ShadowIndex)
{
	ID3D11DeviceContext* DeviceContext = GraphicsContext->GetImmediateContext();
	ID3D11RenderTargetView* RenderTarget = Target->GetRenderTarget();

	FViewport ShadowMapViewport = GenerateViewportFromIndex(ShadowIndex);
	D3D11_VIEWPORT Viewport = ShadowMapViewport.CreateRenderViewport();
	DeviceContext->RSSetViewports(1, &Viewport);

	FShadowInstanceBuffer InstanceBuffer;
	InstanceBuffer.CurrentShadowMap = ShadowIndex;
	DepthOnlyMaterial->GetParameters()->UpdateUniform(DeviceContext, FShadowInstanceBuffer::kBufferName, &InstanceBuffer, sizeof(InstanceBuffer));

	// Render the whole scene with depth only material.
	FInstanceBuffer IdentityBuffer = FInstanceBuffer::Identity;
	FDrawCall Params(DeviceContext, &IdentityBuffer, DepthOnlyMaterial);
	Scene->Draw(Params);
}

void FShadowMapping::UpdateShadowBuffer(IScene* Scene)
{
	ID3D11DeviceContext* DeviceContext = GraphicsContext->GetImmediateContext();

	memset(ShadowBuffer.Get(), 0, sizeof(FShadowBuffer));

	int CurrentIndex = 0;
	// Spotlights first, then directional lights then point lights.
	for (auto SpotLight : Scene->GetSpotLights())
	{
		if (SpotLight.bCastShadow)
		{
			GenerateSpotLightMatrices(SpotLight, ShadowBuffer->LightViewMatrix[CurrentIndex], ShadowBuffer->LightViewProjectionMatrix[CurrentIndex]);
			
			FViewport Viewport = GenerateViewportFromIndex(CurrentIndex);
			ShadowBuffer->LightTextureOffsets[CurrentIndex].x = (float)Viewport.Width / ShadowMapResolution.x;
			ShadowBuffer->LightTextureOffsets[CurrentIndex].y = (float)Viewport.Height / ShadowMapResolution.y;
			ShadowBuffer->LightTextureOffsets[CurrentIndex].z = (float)Viewport.X / ShadowMapResolution.x;
			ShadowBuffer->LightTextureOffsets[CurrentIndex].w = (float)Viewport.Y / ShadowMapResolution.y;

			CurrentIndex++;
		}
	}

	DepthOnlyMaterial->GetParameters()->UpdateUniform(DeviceContext, FShadowBuffer::kBufferName, ShadowBuffer.Get(), sizeof(FShadowBuffer));
}

void FShadowMapping::GenerateSpotLightMatrices(const FSpotlight& SpotLight, glm::mat4& OutViewMatrix, glm::mat4& OutViewProjectionMatrix)
{
	OutViewMatrix = glm::lookAt(SpotLight.Position, SpotLight.Position + SpotLight.Direction, glm::vec3(0.0f, 1.0f, 0.0f));
	float AspectRatio = ShadowMap->GetWidth() / (float)ShadowMap->GetHeight();
	OutViewProjectionMatrix = glm::perspective(glm::radians(90.0f), AspectRatio, 0.1f, 2500.0f) * OutViewMatrix;
}

ID3D11ShaderResourceView* FShadowMapping::GetShadowMap()
{
	return ShadowMap->GetRenderResource();
}

void FShadowMapping::UploadShadowBuffer(ID3D11DeviceContext* DeviceContext)
{
	MaterialManager->UploadGlobalUniformBuffer(FShadowBuffer::kBufferName, ShadowBuffer.Get(), sizeof(FShadowBuffer));
}

FViewport FShadowMapping::GenerateViewportFromIndex(int Index)
{
	FViewport Viewport;

	int MapsPerWidth = FShadowBuffer::kMaxShadowMapNum / 2;

	int X = Index / MapsPerWidth;
	int Y = Index % MapsPerWidth;

	Viewport.Width = ShadowMapResolution.x / MapsPerWidth;
	Viewport.Height = ShadowMapResolution.y / MapsPerWidth;
	Viewport.X = X * Viewport.Width;
	Viewport.Y = Y * Viewport.Height;

	return Viewport;
}



