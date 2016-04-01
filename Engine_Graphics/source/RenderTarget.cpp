#include "RenderTarget.h"
#include "GraphicsContext.h"


FRenderTarget::FRenderTarget(class FGraphicsContext* GraphicsContext, ERenderTargetType RenderType, int Width, int Height,
	DXGI_FORMAT Format, DXGI_FORMAT ViewFormat, DXGI_FORMAT StencilFormat, bool bSupportMips, int SamplesCount) : Width(Width), Height(Height)
{
	ID3D11Device* Device = GraphicsContext->GetDevice();

	this->TextureFormat = Format;

	if (RenderType & ERenderTargetType::Color)
	{
		D3D11_TEXTURE2D_DESC TextureDescription;
		ZeroMemory(&TextureDescription, sizeof(TextureDescription));
		TextureDescription.Width = Width;
		TextureDescription.Height = Height;
		TextureDescription.ArraySize = 1;
		TextureDescription.Format = Format;
		TextureDescription.Usage = D3D11_USAGE_DEFAULT;
		TextureDescription.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		TextureDescription.MipLevels = bSupportMips ? 0 : 1;
		TextureDescription.SampleDesc.Count = SamplesCount;
		TextureDescription.CPUAccessFlags = 0;
		TextureDescription.MiscFlags = bSupportMips ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;

		HRESULT Result;
		Result = Device->CreateTexture2D(&TextureDescription, nullptr, &TextureResource);

		D3D11_SHADER_RESOURCE_VIEW_DESC ViewDescription;
		ZeroMemory(&ViewDescription, sizeof(ViewDescription));
		ViewDescription.Texture2D.MipLevels = -1;
		ViewDescription.Texture2D.MostDetailedMip = 0;
		ViewDescription.Format = ViewFormat;
		ViewDescription.ViewDimension = SamplesCount > 1 ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;

		Result = Device->CreateShaderResourceView(TextureResource, &ViewDescription, &RenderResourceView);
		Result = Device->CreateRenderTargetView(TextureResource, nullptr, &TargetView);
	}

	if (RenderType & ERenderTargetType::Depth)
	{
		D3D11_TEXTURE2D_DESC TextureDescription;
		ZeroMemory(&TextureDescription, sizeof(TextureDescription));
		TextureDescription.Width = Width;
		TextureDescription.Height = Height;
		TextureDescription.ArraySize = 1;
		TextureDescription.Format = StencilFormat;
		TextureDescription.Usage = D3D11_USAGE_DEFAULT;
		TextureDescription.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		TextureDescription.MipLevels = 1;
		TextureDescription.SampleDesc.Count = SamplesCount;
		TextureDescription.CPUAccessFlags = 0;
		TextureDescription.MiscFlags = 0;

		HRESULT Result;
		Result = Device->CreateTexture2D(&TextureDescription, nullptr, &DepthResource);

		D3D11_DEPTH_STENCIL_VIEW_DESC ViewDesc;
		ZeroMemory(&ViewDesc, sizeof(ViewDesc));
		ViewDesc.Format = StencilFormat;
		ViewDesc.ViewDimension = SamplesCount > 0 ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
		ViewDesc.Texture2D.MipSlice = 0;

		Result = Device->CreateDepthStencilView(DepthResource, &ViewDesc, &DepthStencilView);
	}
}

FRenderTarget::~FRenderTarget()
{
	if (RenderResourceView != nullptr)
		RenderResourceView->Release();

	if (TargetView != nullptr)
		TargetView->Release();

	if (DepthStencilView != nullptr)
		DepthStencilView->Release();

	if (TextureResource != nullptr)
		TextureResource->Release();

	if (DepthResource != nullptr)
		DepthResource->Release();
}

void FRenderTarget::GenerateMips(ID3D11DeviceContext* DeviceContext)
{
	DeviceContext->GenerateMips(RenderResourceView);
}

void FRenderTarget::CopyFrom(ID3D11DeviceContext* DeviceContext, FRenderTarget* OtherTarget, int MipLevel)
{
	D3D11_BOX SourceBox;
	SourceBox.back = 0;
	SourceBox.front = 1;
	SourceBox.bottom = 0;
	SourceBox.top = 1;
	SourceBox.left = 0;
	SourceBox.right = 1;
	DeviceContext->CopySubresourceRegion(TextureResource, MipLevel, 0, 0, 0, OtherTarget->TextureResource, MipLevel, &SourceBox);
}

void FRenderTarget::Clear(ID3D11DeviceContext* DeviceContext)
{
	if (TargetView != nullptr)
	{
		const float Colour[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		DeviceContext->ClearRenderTargetView(TargetView, Colour);
	}
	if (DepthStencilView != nullptr)
	{
		DeviceContext->ClearDepthStencilView(DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}
}

void FRenderTarget::ResolveFrom(ID3D11DeviceContext* DeviceContext, FRenderTarget* Target)
{
	DeviceContext->ResolveSubresource(TextureResource, 0, Target->TextureResource, 0, TextureFormat);
}
