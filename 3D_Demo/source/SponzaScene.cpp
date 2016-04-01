#include "MeshComponent.h"
#include "SponzaScene.h"
#include "Input.h"
#include "GraphicsContext.h"
#include "imgui.h"
#include "MeshRenderer.h"
#include "MeshUtility.h"
#include "Transform.h"
#include "SpriteBatch.h"
#include "Profiler.h"
#include "MemorySections.h"
#include "BaseScene.h"
#include "World.h"
#include "Entity.h"
#include "RendererSystem.h"
#include "CameraComponent.h"
#include "LightComponents.h"
#include "CameraSystem.h"
#include "ForwardPlusRenderer.h"
#include "ConfigVariable.h"
#include "EntitySet.h"
#include "LuaState.h"
#include "LuaSceneLoader.h"
#include "LuaDataReader.h"
#include "ImGuiEngine.h"

FSponzaScene::FSponzaScene() : FScene()
{
	SceneRenderer = Make_Unique<FForwardPlusRenderer>(GraphicsContext, MaterialManager);
	ResourceGroup = Make_Unique<FResourceGroup>(GraphicsContext, MaterialManager);
	World = Make_Unique<FWorld>();
}

FSponzaScene::~FSponzaScene()
{
}

void FSponzaScene::Load()
{
	SpriteBatch = Make_Unique<FSpriteBatch>(GraphicsContext, MaterialManager->GetSpriteMaterial(), 50000);
	World->AddSystem(new FRendererSystem(SceneRenderer.Get()));

	FCamera SceneCamera(FViewport(0, 0, GraphicsContext->GetWidth(), GraphicsContext->GetHeight()));
	SceneCamera.SetPerspective(60.0f, (float)GraphicsContext->GetWidth(), (float)GraphicsContext->GetHeight(), 1.0f, 7500.0f);
	SceneCamera.SetYaw(90.0f);
	SceneCamera.SetPosition(glm::vec3(250.0f, 125.0f, 00.0f));

	FEntity* Camera = World->CreateEntity();
	Camera->AddComponent(new FCameraComponent(SceneCamera));

	CameraController = Make_Unique<FFreeFlyCameraController>(Window, InputManager);
	World->AddSystem(new FCameraSystem());
	World->GetSystem<FCameraSystem>()->SetCameraController(CameraController.Get());

	// Load scene data here
	LoadScene(World.Get(), ResourceGroup.Get(), "SponzaScene.lua");
}

void FSponzaScene::Unload()
{
	ResourceGroup->UnloadResources();
}

void FSponzaScene::Render(float DeltaTime)
{
	World->Render();
}

void FSponzaScene::Update(float DeltaTime)
{
	World->GetSystem<FCameraSystem>()->SetActive(InputManager->IsKeyDown(VK_SPACE));

	if (InputManager->IsKeyPressed(VK_F2))
	{
		FEntitySet* LightEntities = World->GetEntitySet({ TClassTypeId<FSpotLightComponent>::Get() });
		FEntitySet* PointLightEntities = World->GetEntitySet({ TClassTypeId<FPointLightComponent>::Get() });
		FEntitySet* ModelEntities = World->GetEntitySet({ TClassTypeId<FMeshComponent>::Get() });
		for (auto Entity : LightEntities->Get())
		{
			Entity->Kill();
		}
		for (auto Entity : PointLightEntities->Get())
		{
			Entity->Kill();
		}
		for (auto Entity : ModelEntities->Get())
		{
			Entity->Kill();
		}

		LoadScene(World.Get(), ResourceGroup.Get(), "SponzaScene.lua", false);
	}

	static bool bProfilerOpened = false;

	if (InputManager->IsKeyPressed(VK_F1))
	{
		bProfilerOpened = !bProfilerOpened;
	}

	if (bProfilerOpened)
	{
		ImGui::ShowProfiler();
	}

	World->Update(DeltaTime);
}