#ifndef GraphicsContext_h__
#define GraphicsContext_h__

#include <windows.h>
#include <d3d11.h>
#include <string>
#include "Viewport.h"
#include "RenderState.h"
#include "UniquePtr.h"

class FGraphicsContext
{
public:
	FGraphicsContext(HWND Hwnd);
	~FGraphicsContext();

	ID3D11RasterizerState* CreateRasterizerState(D3D11_RASTERIZER_DESC* StateDescription);
	ID3D11DepthStencilState* CreateDepthStencilState(D3D11_DEPTH_STENCIL_DESC* StateDescription);
	ID3D11BlendState* CreateBlendState(D3D11_BLEND_DESC* StateDescription);

	void SwapBuffers();

	FRenderStates* GetRenderStates() const { return RenderStates.Get(); }
	ID3D11Device* GetDevice() const { return Device; }
	ID3D11DeviceContext* GetImmediateContext() const { return DeviceContext; }

	int GetWidth() const { return Width; }
	int GetHeight() const { return Height; }

	ID3D11RenderTargetView* GetBackBuffer() const { return BackBuffer; }
	ID3D11DepthStencilView* GetDepthStencil() const { return DepthBuffer; }

#if _DEBUG
	void ReportLiveObjects();
#endif

	//void DoPostProcess(const FViewport& CurrentViewport, ID3D11ShaderResourceView* Source, ID3D11RenderTargetView* Target, class FMaterial* Material, float ViewportWidth = 0.0f, float ViewportHeight = 0.0f);

private:
	void OnResizeWindow(int Width, int Height);
	void Initialize(int Width, int Height);

	ID3D11Device* Device;
	ID3D11DeviceContext* DeviceContext;
	IDXGISwapChain* SwapChain;

	//Backbuffer Target.
	ID3D11RenderTargetView* BackBuffer;
	ID3D11DepthStencilView* DepthBuffer;

	TUniquePtr<FRenderStates> RenderStates;

	HWND Hwnd;
	RECT ScreenRect;

	int Width, Height;
};

void D3DSetDebugName(ID3D11DeviceChild* Resource, const std::string& Name);

#endif // GraphicsContext_h__
