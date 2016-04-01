#include "GraphicsContext.h"
#include "RenderState.h"
#include "D3Dcommon.h"

FGraphicsContext::FGraphicsContext(HWND Hwnd)
{
	this->Hwnd = Hwnd;
	GetClientRect(Hwnd, &ScreenRect);
	Initialize(ScreenRect.right, ScreenRect.bottom); // initialise graphics context
}

FGraphicsContext::~FGraphicsContext()
{
	RenderStates = nullptr;

	BackBuffer->Release();
	DepthBuffer->Release();

	SwapChain->Release();

	DeviceContext->Release();
	DeviceContext->ClearState();
	DeviceContext->Flush();
	DeviceContext = nullptr;

#if _DEBUG
	ReportLiveObjects();
#endif

	Device->Release();
}

void FGraphicsContext::OnResizeWindow(int Width, int Height)
{
	if (Height == 0) // Prevent A Divide By Zero error
	{
		Height = 1;
	}

	this->Width = Width;
	this->Height = Height;

	D3D11_VIEWPORT Viewport;
	Viewport.Height = (float)Height;
	Viewport.Width = (float)Width;
	Viewport.TopLeftX = 0.0f;
	Viewport.TopLeftY = 0.0f;
	Viewport.MinDepth = 0.0f;
	Viewport.MaxDepth = 1.0f;

	DeviceContext->RSSetViewports(1, &Viewport);
}

void FGraphicsContext::Initialize(int Width, int Height)
{
	this->Width = Width;
	this->Height = Height;

	DXGI_SWAP_CHAIN_DESC SwapChainDescriptor;
	ZeroMemory(&SwapChainDescriptor, sizeof(DXGI_SWAP_CHAIN_DESC));

	SwapChainDescriptor.BufferCount = 1;
	SwapChainDescriptor.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SwapChainDescriptor.BufferDesc.RefreshRate.Numerator = 60;
	SwapChainDescriptor.BufferDesc.RefreshRate.Denominator = 1;
	SwapChainDescriptor.BufferDesc.Width = Width;
	SwapChainDescriptor.BufferDesc.Height = Height;
	SwapChainDescriptor.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	SwapChainDescriptor.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChainDescriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDescriptor.OutputWindow = Hwnd;
	SwapChainDescriptor.SampleDesc.Count = 1;
	SwapChainDescriptor.Windowed = true;

	UINT DeviceCreationFlags = NULL;

	DeviceCreationFlags |= D3D11_CREATE_DEVICE_DEBUG;

	D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		DeviceCreationFlags,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&SwapChainDescriptor,
		&SwapChain,
		&Device,
		NULL,
		&DeviceContext);

	//Get pointer to backbuffer
	ID3D11Texture2D* BackBufferTexture;
	SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&BackBufferTexture);
	Device->CreateRenderTargetView(BackBufferTexture, NULL, &BackBuffer);
	BackBufferTexture->Release();

	ID3D11Texture2D* DepthStencilTexture;

	D3D11_TEXTURE2D_DESC DepthBufferDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc;

	// Initialize the description of the depth buffer.
	ZeroMemory(&DepthBufferDesc, sizeof(DepthBufferDesc));

	// Set up the description of the depth buffer.
	DepthBufferDesc.Width = Width;
	DepthBufferDesc.Height = Height;
	DepthBufferDesc.MipLevels = 1;
	DepthBufferDesc.ArraySize = 1;
	DepthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DepthBufferDesc.SampleDesc.Count = 1;
	DepthBufferDesc.SampleDesc.Quality = 0;
	DepthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	DepthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	DepthBufferDesc.CPUAccessFlags = 0;
	DepthBufferDesc.MiscFlags = 0;

	// Create the texture for the depth buffer using the filled out description.
	Device->CreateTexture2D(&DepthBufferDesc, NULL, &DepthStencilTexture);

	// Initialize the depth stencil view.
	ZeroMemory(&DepthStencilViewDesc, sizeof(DepthStencilViewDesc));

	// Set up the depth stencil view description.
	DepthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	DepthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view.
	Device->CreateDepthStencilView(DepthStencilTexture, &DepthStencilViewDesc, &DepthBuffer);

	// Bind the render target view and depth stencil buffer to the output render pipeline.
	DeviceContext->OMSetRenderTargets(1, &BackBuffer, DepthBuffer);

	D3D11_VIEWPORT Viewport;
	Viewport.Height = (float)Height;
	Viewport.Width = (float)Width;
	Viewport.TopLeftX = 0;
	Viewport.TopLeftY = 0;
	Viewport.MinDepth = 0.0f;
	Viewport.MaxDepth = 1.0f;

	DeviceContext->RSSetViewports(1, &Viewport);
	
	D3DSetDebugName(BackBuffer, "BackBuffer View");
	D3DSetDebugName(DepthBuffer, "Depth Buffer View");

	RenderStates = Make_Unique<FRenderStates>(this);

	// Set default render states.
	DeviceContext->RSSetState(RenderStates->SolidRasterizer);
	DeviceContext->OMSetDepthStencilState(RenderStates->LessDepthState, 0);
	DeviceContext->OMSetBlendState(RenderStates->OpaqueBlendState, nullptr, 0xFFFFFFFF);
}

void FGraphicsContext::SwapBuffers()
{
	SwapChain->Present(1, 0);
}

ID3D11RasterizerState* FGraphicsContext::CreateRasterizerState(D3D11_RASTERIZER_DESC* StateDescription)
{
	ID3D11RasterizerState* State;
	Device->CreateRasterizerState(StateDescription, &State);
	return State;
}

ID3D11DepthStencilState* FGraphicsContext::CreateDepthStencilState(D3D11_DEPTH_STENCIL_DESC* StateDescription)
{
	ID3D11DepthStencilState* State;
	Device->CreateDepthStencilState(StateDescription, &State);
	return State;
}

ID3D11BlendState* FGraphicsContext::CreateBlendState(D3D11_BLEND_DESC* StateDescription)
{
	ID3D11BlendState* State;
	Device->CreateBlendState(StateDescription, &State);
	return State;
}

#if _DEBUG
void FGraphicsContext::ReportLiveObjects()
{
	if (DeviceContext != nullptr)
	{
		DeviceContext->ClearState();
		DeviceContext->Flush();
	}

	ID3D11Debug* Debug;
	Device->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&Debug));
	if (Debug != nullptr)
	{
		Debug->ReportLiveDeviceObjects(D3D11_RLDO_SUMMARY | D3D11_RLDO_DETAIL);
	}
}
#endif

void D3DSetDebugName(ID3D11DeviceChild* Resource, const std::string& Name)
{
#if _DEBUG
	Resource->SetPrivateData(WKPDID_D3DDebugObjectName, Name.size(), Name.c_str());
#endif
}
