#include "RenderState.h"
#include "GraphicsContext.h"


FRenderStates::FRenderStates(FGraphicsContext* GraphicsContext)
{
	// Rasterizer States:
	D3D11_RASTERIZER_DESC CommonRasterizerDesc;
	CommonRasterizerDesc.AntialiasedLineEnable = true;
	CommonRasterizerDesc.DepthClipEnable = false;
	CommonRasterizerDesc.DepthBias = 0;
	CommonRasterizerDesc.DepthBiasClamp = 0.0f;
	CommonRasterizerDesc.FrontCounterClockwise = true;
	CommonRasterizerDesc.MultisampleEnable = false;
	CommonRasterizerDesc.ScissorEnable = false;
	CommonRasterizerDesc.SlopeScaledDepthBias = 0.0f;
	CommonRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	CommonRasterizerDesc.CullMode = D3D11_CULL_NONE;

	D3D11_RASTERIZER_DESC SolidDesc = CommonRasterizerDesc;
	SolidDesc.FillMode = D3D11_FILL_SOLID;
	SolidDesc.CullMode = D3D11_CULL_BACK;

	D3D11_RASTERIZER_DESC SolidNoCullDesc = CommonRasterizerDesc;
	SolidNoCullDesc.FillMode = D3D11_FILL_SOLID;
	SolidNoCullDesc.CullMode = D3D11_CULL_NONE;

	D3D11_RASTERIZER_DESC WireframeDesc = CommonRasterizerDesc;
	WireframeDesc.FillMode = D3D11_FILL_WIREFRAME;

	SolidRasterizer = GraphicsContext->CreateRasterizerState(&SolidDesc);
	SolidNoCullRasterizer = GraphicsContext->CreateRasterizerState(&SolidNoCullDesc);
	WireframeRasterizer = GraphicsContext->CreateRasterizerState(&WireframeDesc);

	// Depth Stencil States:
	D3D11_DEPTH_STENCIL_DESC DepthStencilDesc;
	ZeroMemory(&DepthStencilDesc, sizeof(DepthStencilDesc));
	DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	DepthStencilDesc.DepthEnable = true;
	DepthStencilDesc.StencilEnable = false;
	DepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	D3D11_DEPTH_STENCIL_DESC NoDepthStencilDesc;
	ZeroMemory(&NoDepthStencilDesc, sizeof(DepthStencilDesc));
	NoDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	NoDepthStencilDesc.DepthEnable = false;
	NoDepthStencilDesc.StencilEnable = false;
	NoDepthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;

	LessDepthState = GraphicsContext->CreateDepthStencilState(&DepthStencilDesc);
	NoDepthState = GraphicsContext->CreateDepthStencilState(&NoDepthStencilDesc);

	// Blend States:
	D3D11_BLEND_DESC CommonBlend;
	CommonBlend.IndependentBlendEnable = false;
	CommonBlend.AlphaToCoverageEnable = false;

	D3D11_BLEND_DESC AdditiveBlend = CommonBlend; // Add colours but replace alpha
	for (int i = 0; i < 8; i++)
	{
		AdditiveBlend.RenderTarget[i].BlendEnable = true;
		AdditiveBlend.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		AdditiveBlend.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
		AdditiveBlend.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		AdditiveBlend.RenderTarget[i].SrcBlend = D3D11_BLEND_ONE;
		AdditiveBlend.RenderTarget[i].DestBlend = D3D11_BLEND_ONE;
		AdditiveBlend.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
		AdditiveBlend.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ZERO;
	}

	D3D11_BLEND_DESC AlphaBlend = CommonBlend; // Replace alpha but blend src color
	for (int i = 0; i < 8; i++)
	{
		AlphaBlend.RenderTarget[i].BlendEnable = true;
		AlphaBlend.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		AlphaBlend.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
		AlphaBlend.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		AlphaBlend.RenderTarget[i].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		AlphaBlend.RenderTarget[i].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		AlphaBlend.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
		AlphaBlend.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ZERO;
	}

	D3D11_BLEND_DESC PremultipliedAlphaBlend = CommonBlend;
	for (int i = 0; i < 8; i++)
	{
		PremultipliedAlphaBlend.RenderTarget[i].BlendEnable = true;
		PremultipliedAlphaBlend.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		PremultipliedAlphaBlend.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
		PremultipliedAlphaBlend.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		PremultipliedAlphaBlend.RenderTarget[i].SrcBlend = D3D11_BLEND_ONE;
		PremultipliedAlphaBlend.RenderTarget[i].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		PremultipliedAlphaBlend.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
		PremultipliedAlphaBlend.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ZERO;
	}

	D3D11_BLEND_DESC OpaqueBlend = CommonBlend;
	for (int i = 0; i < 8; i++)
	{
		OpaqueBlend.RenderTarget[i].BlendEnable = true;
		OpaqueBlend.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		OpaqueBlend.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
		OpaqueBlend.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		OpaqueBlend.RenderTarget[i].SrcBlend = D3D11_BLEND_ONE;
		OpaqueBlend.RenderTarget[i].DestBlend = D3D11_BLEND_ZERO;
		OpaqueBlend.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
		OpaqueBlend.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ZERO;
	}

	AlphaBlendState = GraphicsContext->CreateBlendState(&AlphaBlend);
	PremultipliedAlphaBlendState = GraphicsContext->CreateBlendState(&PremultipliedAlphaBlend);
	OpaqueBlendState = GraphicsContext->CreateBlendState(&OpaqueBlend);
	AdditiveBlendState = GraphicsContext->CreateBlendState(&AdditiveBlend);

	const float kBorderColour[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	// Sampler States
	ID3D11Device* Device = GraphicsContext->GetDevice();
	CD3D11_SAMPLER_DESC NearestDesc(
		D3D11_FILTER_MIN_MAG_MIP_POINT,
		D3D11_TEXTURE_ADDRESS_CLAMP,
		D3D11_TEXTURE_ADDRESS_CLAMP,
		D3D11_TEXTURE_ADDRESS_CLAMP,
		0.0f,
		1,
		D3D11_COMPARISON_NEVER,
		kBorderColour,
		0,
		D3D11_FLOAT32_MAX);

	CD3D11_SAMPLER_DESC LinearDesc(
		D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
		D3D11_TEXTURE_ADDRESS_CLAMP,
		D3D11_TEXTURE_ADDRESS_CLAMP,
		D3D11_TEXTURE_ADDRESS_CLAMP,
		0.0f,
		1,
		D3D11_COMPARISON_NEVER,
		kBorderColour,
		0,
		D3D11_FLOAT32_MAX);

	CD3D11_SAMPLER_DESC BilinearDesc(
		D3D11_FILTER_ANISOTROPIC,
		D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_TEXTURE_ADDRESS_WRAP,
		0.0f,
		1,
		D3D11_COMPARISON_NEVER,
		kBorderColour,
		-D3D11_FLOAT32_MAX,
		D3D11_FLOAT32_MAX);

	Device->CreateSamplerState(&NearestDesc, &SimpleSampler);
	Device->CreateSamplerState(&BilinearDesc, &TextureSampler);
	Device->CreateSamplerState(&LinearDesc, &LinearSampler);

	// Set Debug Names
	D3DSetDebugName(SolidRasterizer, "Solid");
	D3DSetDebugName(SolidNoCullRasterizer, "Solid No Cull");
	D3DSetDebugName(WireframeRasterizer, "Wireframe");
	D3DSetDebugName(LessDepthState, "Depth - Less");
	D3DSetDebugName(AlphaBlendState, "Alpha Blending");
	D3DSetDebugName(PremultipliedAlphaBlendState, "Premultiplied Alpha Blending");
	D3DSetDebugName(OpaqueBlendState, "Opaque Blending");
	D3DSetDebugName(AdditiveBlendState, "Additive Blending");
	D3DSetDebugName(SimpleSampler, "Nearest Sampler");
	D3DSetDebugName(TextureSampler, "Bilinear Sampler");
	D3DSetDebugName(LinearSampler, "Linear Sampler");
}

FRenderStates::~FRenderStates()
{
	SolidNoCullRasterizer->Release();
	SolidRasterizer->Release();
	WireframeRasterizer->Release();

	NoDepthState->Release();
	LessDepthState->Release();

	AlphaBlendState->Release();
	PremultipliedAlphaBlendState->Release();
	OpaqueBlendState->Release();
	AdditiveBlendState->Release();

	SimpleSampler->Release();
	TextureSampler->Release();
}
