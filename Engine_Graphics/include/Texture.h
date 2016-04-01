#ifndef Texture_h__
#define Texture_h__

#include <string>
#include <cstdint>
#include <d3d11.h>
#include "Archive.h"

class FGraphicsContext;

enum ETextureFiltering
{
	Point,
	Linear,
	Trilinear,
};

class FTexture
{
public:
	FTexture(FGraphicsContext* GraphicsContext, int Width, int Height, void* Data, DXGI_FORMAT TextureFormat = DXGI_FORMAT_R8G8B8A8_UNORM, int bytesPerPixel = 4, bool bAllowCpuWriteAccess = false);
	FTexture(FGraphicsContext* GraphicsContext, IArchive* Archive, bool bIsGammaSpace);
	~FTexture();

	void SetName(const std::string& Name);

	void UploadTextureData(ID3D11DeviceContext* DeviceContext, void* Data, int SizeInBytes);

	ID3D11ShaderResourceView* GetResourceView() const;

	int GetWidth() const { return Width; }
	int GetHeight() const { return Height; }
private:
	void CreateResource(ID3D11Device* Device, int Width, int Height, void* Data, DXGI_FORMAT TextureFormat = DXGI_FORMAT_R8G8B8A8_UNORM, int bytesPerPixel = 4, bool bAllowCpuWriteAccess = false);

	ID3D11Texture2D* TextureResource = nullptr;
	ID3D11ShaderResourceView* ResourceView = nullptr;

	ETextureFiltering TextureFiltering;
	int Width;
	int Height;
};

#endif // Texture_h__
