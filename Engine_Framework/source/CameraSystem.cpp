#include "CameraSystem.h"
#include "CameraComponent.h"
#include "EntitySet.h"
#include "Entity.h"

void FCameraSystem::OnAdded()
{
	CameraEntities = GetEntitySet({ TYPE_OF(FCameraComponent) });
}

void FCameraSystem::Update(float DeltaTime)
{
	if (CameraController == nullptr || bIsActive == false)
		return;

	for (FEntity* Entity : CameraEntities->Get())
	{
		auto CameraComponent = Entity->GetComponent<FCameraComponent>();
		if (CameraComponent->bIsActive)
		{
			CameraController->Update(DeltaTime, &CameraComponent->Camera);
		}
	}
}

void FCameraSystem::SetCameraController(ICameraController* CameraController)
{
	this->CameraController = CameraController;
}

void FCameraSystem::SetActive(bool bActive)
{
	this->bIsActive = bActive;
}

