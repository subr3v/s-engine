#include "Entity.h"
#include <algorithm>
#include "Component.h"
#include "World.h"

FEntity::FEntity(FWorld* World) : World(World), bMarkedForDestruction(false)
{

}

FEntity::~FEntity()
{
	FProfiler::Get().ProfileCounter("Gameplay", "Component Count", false, -(s32)Components.size());
	for (auto& Pair : Components)
	{
		delete Pair.second;
	}
	Components.clear();
	World = nullptr;
}

FComponent* FEntity::AddComponent(FComponent* Component)
{
	FProfiler::Get().ProfileCounter("Gameplay", "Component Count", false, 1);
	Components[Component->GetClassInfo()] = Component;
	World->NotifyEntityComponentsChanged(this);
	return Component;
}

bool FEntity::RemoveComponent(FComponent* Component)
{
	FProfiler::Get().ProfileCounter("Gameplay", "Component Count", false, -1);
	if (HasComponent(Component->GetClassInfo()))
	{
		Components[Component->GetClassInfo()] = nullptr;
		delete Component;
		World->NotifyEntityComponentsChanged(this);
		return true;
	}
	return false;
}

FComponent* FEntity::GetComponent(const FClassInfo* ClassInfo)
{
	return Components[ClassInfo];
}

bool FEntity::HasComponent(const FClassInfo* ClassInfo)
{
	return Components[ClassInfo] != nullptr;
}

void FEntity::Kill()
{
	if (bMarkedForDestruction == false)
	{
		bMarkedForDestruction = true;
		// TODO: Maybe dispatch an event so that things know they're going to be destroyed soon?
	}
}

void FEntity::SetName(const std::string& Name)
{
	this->Name = Name;

}
