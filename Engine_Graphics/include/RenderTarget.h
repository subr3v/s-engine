#ifndef RenderTarget_h__
#define RenderTarget_h__

#include <d3d11.h>

enum class ERenderTargetType : unsigned char
{
	Color = 2,
	Depth = 4,
};

static ERenderTargetType operator|(ERenderTargetType A, ERenderTargetType B)
{
	return (ERenderTargetType)((int)A | (int)B);
}

static bool operator&(ERenderTargetType A, ERenderTargetType B)
{
	return ((int)A & (int)B) != 0;
}

class FRenderTarget
{
public:
	FRenderTarget(class FGraphicsContext* GraphicsContext, ERenderTargetType RenderType, int Width, int Height,
		DXGI_FORMAT Format, DXGI_FORMAT ViewFormat, DXGI_FORMAT StencilFormat = DXGI_FORMAT_D16_UNORM, bool bSupportMips = false, int SamplesCount = 1);
	~FRenderTarget();

	void ResolveFrom(ID3D11DeviceContext* DeviceContext, FRenderTarget* Target);
	void Clear(ID3D11DeviceContext* DeviceContext);
	void GenerateMips(ID3D11DeviceContext* DeviceContext);
	void CopyFrom(ID3D11DeviceContext* DeviceContext, FRenderTarget* OtherTarget, int MipLevel);

	ID3D11RenderTargetView* GetRenderTarget() const { return TargetView; }
	ID3D11DepthStencilView* GetDepthStencil() const { return DepthStencilView; }
	ID3D11ShaderResourceView* GetRenderResource() const { return RenderResourceView; }

	int GetWidth() const { return Width; }
	int GetHeight() const { return Height; }
private:
	ID3D11Texture2D* TextureResource = nullptr;
	ID3D11Texture2D* DepthResource = nullptr;
	ID3D11ShaderResourceView* RenderResourceView = nullptr;
	ID3D11RenderTargetView* TargetView = nullptr;
	ID3D11DepthStencilView* DepthStencilView = nullptr;
	DXGI_FORMAT TextureFormat;

	int Width;
	int Height;
};


#endif // RenderTarget_h__
