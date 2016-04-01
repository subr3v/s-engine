#include "Texture.h"
#include "GraphicsContext.h"
#include <d3d11.h>
#include "TextureData.h"
#include <fstream>
#include "FileArchive.h"
#include "ZipArchive.h"

FTexture::FTexture(FGraphicsContext* GraphicsContext, int Width, int Height, void* Data, DXGI_FORMAT TextureFormat, int bytesPerPixel, bool bAllowCpuWriteAccess)
{
	CreateResource(GraphicsContext->GetDevice(), Width, Height, Data, TextureFormat, bytesPerPixel, bAllowCpuWriteAccess);
}

FTexture::FTexture(FGraphicsContext* GraphicsContext, IArchive* Archive, bool bIsGammaSpace)
{
	FTextureData TextureData;
	TextureData.Serialize(FSerializer(Archive, true, false));
	CreateResource(GraphicsContext->GetDevice(), TextureData.Width, TextureData.Height, TextureData.PixelData.Data(), bIsGammaSpace ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM);
}

FTexture::~FTexture()
{
	FProfiler::Get().ProfileCounter("Rendering", "Textures Count", false, -1);
	if (ResourceView != nullptr)
		ResourceView->Release();
	if (TextureResource != nullptr)
		TextureResource->Release();
}

void FTexture::CreateResource(ID3D11Device* Device, int Width, int Height, void* Data, DXGI_FORMAT TextureFormat, int bytesPerPixel, bool bAllowCpuWriteAccess)
{
	this->Width = Width;
	this->Height = Height;

	bool bGenerateMipmaps = bytesPerPixel == 4 && (TextureFormat == DXGI_FORMAT_R8G8B8A8_UNORM_SRGB) || (TextureFormat == DXGI_FORMAT_R8G8B8A8_UNORM);

	D3D11_TEXTURE2D_DESC TextureDescription;
	ZeroMemory(&TextureDescription, sizeof(TextureDescription));
	TextureDescription.Width = Width;
	TextureDescription.Height = Height;
	TextureDescription.ArraySize = 1;
	TextureDescription.Format = TextureFormat;
	TextureDescription.Usage = bAllowCpuWriteAccess == false ? D3D11_USAGE_DEFAULT : D3D11_USAGE_DYNAMIC;
	TextureDescription.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	TextureDescription.MipLevels = bGenerateMipmaps ? 0 : 1;
	TextureDescription.SampleDesc.Count = 1;
	TextureDescription.CPUAccessFlags = bAllowCpuWriteAccess ? D3D11_CPU_ACCESS_WRITE : 0;

	// We only support mipmaps for this kind of format.
	if (bGenerateMipmaps && bAllowCpuWriteAccess == false)
	{
		TextureDescription.BindFlags |= D3D11_BIND_RENDER_TARGET;
		TextureDescription.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
	}

	ID3D11DeviceContext* Context;
	Device->GetImmediateContext(&Context);

	HRESULT Result;
	Result = Device->CreateTexture2D(&TextureDescription, nullptr, &TextureResource);

	if (Data != nullptr)
		Context->UpdateSubresource(TextureResource, 0, nullptr, Data, bytesPerPixel * Width, bytesPerPixel * Width * Height);

	D3D11_SHADER_RESOURCE_VIEW_DESC ViewDescription;
	ZeroMemory(&ViewDescription, sizeof(ViewDescription));
	ViewDescription.Texture2D.MipLevels = -1;
	ViewDescription.Texture2D.MostDetailedMip = 0;	
	ViewDescription.Format = TextureDescription.Format;
	ViewDescription.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

	Result = Device->CreateShaderResourceView(TextureResource, &ViewDescription, &ResourceView);
	if (bGenerateMipmaps)
	{
		Context->GenerateMips(ResourceView);
	}

	FProfiler::Get().ProfileCounter("Rendering", "Textures Count", false, 1);
}

ID3D11ShaderResourceView* FTexture::GetResourceView() const
{
	return ResourceView;
}

void FTexture::UploadTextureData(ID3D11DeviceContext* DeviceContext, void* Data, int SizeInBytes)
{
	D3D11_MAPPED_SUBRESOURCE Resource;
	if (DeviceContext->Map(TextureResource, 0, D3D11_MAP_WRITE_DISCARD, 0, &Resource) == S_OK)
	{
		memcpy(Resource.pData, Data, SizeInBytes);
		DeviceContext->Unmap(TextureResource, 0);
	}
}

void FTexture::SetName(const std::string& Name)
{
	if (TextureResource != nullptr)
	{
		D3DSetDebugName(TextureResource, Name);
		D3DSetDebugName(ResourceView, Name + " View");
	}
}
