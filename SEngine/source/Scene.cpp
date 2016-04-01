#include "Scene.h"
#include "MaterialManager.h"
#include "GraphicsContext.h"
#include "Engine.h"

FScene::FScene()
{
	InputManager = SEngine::GetInput();
	Window = SEngine::GetWindow();
	GraphicsContext = SEngine::GetGraphicsContext();
	MaterialManager = SEngine::GetMaterialManager();
}

FScene::~FScene()
{
	InputManager = nullptr;
	GraphicsContext = nullptr;
	Window = nullptr;
	MaterialManager = nullptr;
}

void FScene::Load()
{

}

void FScene::Unload()
{

}

void FScene::Update(float DeltaTime)
{

}

void FScene::Render(float DeltaTime)
{
	const float ClearColour[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	ID3D11RenderTargetView* RenderTarget = GraphicsContext->GetBackBuffer();
	GraphicsContext->GetImmediateContext()->OMSetRenderTargets(1, &RenderTarget, GraphicsContext->GetDepthStencil());
	GraphicsContext->GetImmediateContext()->ClearRenderTargetView(RenderTarget, ClearColour);
	GraphicsContext->GetImmediateContext()->ClearDepthStencilView(GraphicsContext->GetDepthStencil(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	FRenderFrameData DefaultFrameData;
	memset(&DefaultFrameData, 0, sizeof(DefaultFrameData));

	MaterialManager->UpdateMaterialBuffers(DefaultFrameData, GraphicsContext);
}