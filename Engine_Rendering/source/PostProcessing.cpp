#include "PostProcessing.h"
#include "FastFullscreenPass.h"
#include "ConstantBufferTypes.h"
#include "GraphicsContext.h"
#include "MaterialParameters.h"
#include "Material.h"
#include "RenderState.h"

void FPostProcessing::Apply(class FGraphicsContext*  GraphicsContext, const FViewport& CurrentViewport, ID3D11ShaderResourceView* Source, ID3D11RenderTargetView* Target, class FMaterial* Material, float ViewportWidth /*= 0.0f*/, float ViewportHeight /*= 0.0f*/)
{
	if (ViewportWidth == 0.0f || ViewportHeight == 0.0f)
	{
		ViewportWidth = (float)GraphicsContext->GetWidth();
		ViewportHeight = (float)GraphicsContext->GetHeight();
	}
	 
	ID3D11DeviceContext* DeviceContext = GraphicsContext->GetImmediateContext();
	FViewport PostProcessViewport(0, 0, (s32)ViewportWidth, (s32)ViewportHeight);
	 
	if (Target == GraphicsContext->GetBackBuffer())
	{
	 	PostProcessViewport = CurrentViewport;
	}
	 
	D3D11_VIEWPORT Viewport = PostProcessViewport.CreateRenderViewport();
	DeviceContext->RSSetViewports(1, &Viewport);
	DeviceContext->OMSetRenderTargets(1, &Target, nullptr);

	if (Source != nullptr)
	{
	 	Material->GetParameters()->SetResource(FTextureNames::kDiffuseMap, Source);
	}
	
	// Null depth stencil buffer cause we don't need it. (at least for now, some passes might actually require a stencil)
	FDrawCall Params(DeviceContext, nullptr, Material);
	FFastFullscreenPass::Render(Params);
}


