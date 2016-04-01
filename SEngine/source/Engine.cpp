#include "Engine.h"
#include "ConfigVariableManager.h"
#include "ImGuiEngine.h"
#include "NetworkEventHandler.h"
#include "MaterialManager.h"
#include "Scene.h"
#include "EngineVariables.h"
#include "Profiler.h"

namespace SEngine
{

struct FEngineModules
{
	FTimer* Timer;
	FWindow* Window;
	FInput* Input;
	FNetworkEventHandler* NetworkEventHandler;
	FGraphicsContext* GraphicsContext;
	FMaterialManager* MaterialManager;
	FImGuiImpl* ImGui;
	FScene* CurrentScene;
	FScene* NextScene;
};

FEngineModules EngineModules;

static void InitCore(HINSTANCE hInstance, int nCmdShow)
{
	FConfigVariableManager::Get().Initialise();

	// Startup socket version 2.2
	WSADATA w;
	int error = WSAStartup(0x0202, &w);

	int32 Width = EngineVariables::WindowWidth.AsInt();
	int32 Height = EngineVariables::WindowHeight.AsInt();
	std::string& WindowTitle = EngineVariables::WindowTitle.AsString();

	// Core Module
	EngineModules.Window = new FWindow(hInstance, nCmdShow, Width, Height);
	EngineModules.Input = new FInput();
	EngineModules.NetworkEventHandler = new FNetworkEventHandler();
	EngineModules.Timer = new FTimer();

	EngineModules.Window->SetTitle(WindowTitle);

	EngineModules.Window->RegisterEventHandler(EngineModules.Input);
	EngineModules.Window->RegisterEventHandler(EngineModules.NetworkEventHandler);
}

static void ShutdownCore()
{
	delete EngineModules.Timer;
	delete EngineModules.NetworkEventHandler;
	delete EngineModules.Input;
	delete EngineModules.Window;
	// Shuts down sockets
	WSACleanup();
	FConfigVariableManager::Get().Shutdown();
}

static void InitRendering()
{
	EngineModules.GraphicsContext = new FGraphicsContext(EngineModules.Window->GetHandle());
	EngineModules.MaterialManager = new FMaterialManager(EngineModules.GraphicsContext);
	EngineModules.MaterialManager->LoadMaterials();
}

static void ShutdownRendering()
{
	delete EngineModules.MaterialManager;
	delete EngineModules.GraphicsContext;
}

static void InitImGui()
{
	EngineModules.ImGui = new FImGuiImpl(EngineModules.GraphicsContext, EngineModules.MaterialManager, EngineModules.Input);
	EngineModules.ImGui->Init();
}

static void ShutdownImGui()
{
	delete EngineModules.ImGui;
}

void Init(HINSTANCE hInstance, int nCmdShow)
{
	// Make sure everything is zero-ed out.
	memset(&EngineModules, 0, sizeof(FEngineModules));

	InitCore(hInstance, nCmdShow);
	InitRendering();
	InitImGui();
}

void Shutdown()
{
	ShutdownImGui();
	ShutdownRendering();
	ShutdownCore();
}

void Run(FScene* StartingScene)
{
	ChangeScene(StartingScene);

	float DeltaTime = 0.0f;

	FTimer* Timer = EngineModules.Timer;
	Timer->Start();
	while (EngineModules.Window->HandleEvents())
	{
		Timer->Restart();
		DeltaTime = Timer->GetElapsedTimeSeconds();

		FProfiler::Get().PushSection("Engine", "Update");
		EngineModules.ImGui->Update(DeltaTime);
		if (EngineModules.CurrentScene != nullptr)
		{
			EngineModules.CurrentScene->Update(DeltaTime);
			FProfiler::Get().PopSection();
			EngineModules.CurrentScene->Render(DeltaTime);
		}
		else
		{
			FProfiler::Get().PopSection();
		}

		EngineModules.ImGui->Render();
		EngineModules.GraphicsContext->SwapBuffers();

		if (EngineModules.NextScene != nullptr)
		{
			if (EngineModules.CurrentScene != nullptr)
			{
				EngineModules.CurrentScene->Unload();
				delete EngineModules.CurrentScene;
			}

			EngineModules.CurrentScene = EngineModules.NextScene;
			EngineModules.CurrentScene->Load();

			EngineModules.NextScene = nullptr;
		}

		EngineModules.Input->SignalFrameEnded();
		FProfiler::Get().OnFrameEnded();
	}

	if (EngineModules.CurrentScene != nullptr)
	{
		EngineModules.CurrentScene->Unload();
		delete EngineModules.CurrentScene;
		EngineModules.CurrentScene = nullptr;
	}
}

FWindow* SEngine::GetWindow()
{
	return EngineModules.Window;
}

FGraphicsContext* SEngine::GetGraphicsContext()
{
	return EngineModules.GraphicsContext;
}

FMaterialManager* SEngine::GetMaterialManager()
{
	return EngineModules.MaterialManager;
}

FInput* SEngine::GetInput()
{
	return EngineModules.Input;
}

FTimer* SEngine::GetTimer()
{
	return EngineModules.Timer;
}

FNetworkEventHandler* SEngine::GetNetworkEventHandler()
{
	return EngineModules.NetworkEventHandler;
}

void SEngine::ChangeScene(FScene* NextScene)
{
	EngineModules.NextScene = NextScene;
}

}
