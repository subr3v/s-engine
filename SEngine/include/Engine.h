#ifndef Engine_h__
#define Engine_h__

#include "Window.h"
#include "GraphicsContext.h"
#include "MaterialManager.h"
#include "Input.h"
#include "Timer.h"
#include "Scene.h"
#include "NetworkEventHandler.h"

namespace SEngine
{
	void Init(HINSTANCE hInstance, int nCmdShow);
	void Run(FScene* StartingScene);
	void Shutdown();
	void ChangeScene(FScene* NextScene);

	FWindow* GetWindow();
	FGraphicsContext* GetGraphicsContext();
	FMaterialManager* GetMaterialManager();
	FInput* GetInput();
	FTimer* GetTimer();
	FNetworkEventHandler* GetNetworkEventHandler();
}

#endif // Engine_h__
