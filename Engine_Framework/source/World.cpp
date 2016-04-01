#include "World.h"
#include "System.h"
#include <algorithm>
#include "Entity.h"
#include "EntitySet.h"

FWorld::FWorld()
{

}

FWorld::~FWorld()
{
	FProfiler::Get().ResetCounter("Gameplay", "Entity Count", false);
	FProfiler::Get().ResetCounter("Gameplay", "System Count", false);
	for (auto Entity : Entities)
	{
		delete Entity;
	}
	Entities.Clear();

	for (auto System : Systems)
	{
		delete System;
	}
	Systems.Clear();

	for (auto EntitySet : EntitySets)
	{
		delete EntitySet;
	}
	EntitySets.Clear();
}

FEntity* FWorld::CreateEntity()
{
	FProfiler::Get().ProfileCounter("Gameplay", "Entity Count", false, 1);
	FEntity* Entity = new FEntity(this);
	Entities.Add(Entity);
	NotifyEntityAdded(Entity);
	return Entity;
}

FEntity* FWorld::CreateEntity(std::initializer_list<FComponent*> Components)
{
	FProfiler::Get().ProfileCounter("Gameplay", "Entity Count", false, 1);
	FEntity* Entity = new FEntity(this);
	for (auto Component : Components)
	{
		Entity->AddComponent(Component);
	}
	Entities.Add(Entity);
	NotifyEntityAdded(Entity);
	return Entity;

}

void FWorld::AddSystem(FGameSystem* GameSystem)
{
	FProfiler::Get().ProfileCounter("Gameplay", "System Count", false, 1);
	Systems.Add(GameSystem);
	GameSystem->World = this;
	GameSystem->OnAdded();
}

void FWorld::RemoveSystem(FGameSystem* GameSystem)
{
	FProfiler::Get().ProfileCounter("Gameplay", "System Count", false, -1);
	Systems.Remove(GameSystem);
	GameSystem->World = nullptr;
	GameSystem->OnRemoved();
	delete GameSystem;
}

void FWorld::PreUpdate(float DeltaTime)
{
	for (auto System : Systems)
	{
		System->PreUpdate(DeltaTime);
	}
}

void FWorld::OnUpdate(float DeltaTime)
{
	for (auto System : Systems)
	{
		System->Update(DeltaTime);
	}
}

void FWorld::PostUpdate(float DeltaTime)
{
	for (auto System : Systems)
	{
		System->PostUpdate(DeltaTime);
	}
}

void FWorld::Update(float DeltaTime)
{
	// Checks for entities that are marked for deletion and removes them before everything else gets triggered.
	CheckPendingEntities();
	PreUpdate(DeltaTime);
	OnUpdate(DeltaTime);
	PostUpdate(DeltaTime);
}

void FWorld::PreRender()
{
	for (auto System : Systems)
	{
		System->PreRender();
	}
}

void FWorld::OnRender()
{
	for (auto System : Systems)
	{
		System->Render();
	}
}

void FWorld::PostRender()
{
	for (auto System : Systems)
	{
		System->PostRender();
	}
}

void FWorld::Render()
{
	PreRender();
	OnRender();
	PostRender();
}

void FWorld::CheckPendingEntities()
{
	bool bEntitiesPending = true;
	while (bEntitiesPending)
	{
		// Might not be the most efficient way but it works.
		bEntitiesPending = false;
		for (auto Entity : Entities)
		{
			if (Entity->bMarkedForDestruction)
			{
				FProfiler::Get().ProfileCounter("Gameplay", "Entity Count", false, -1);
				bEntitiesPending = true;
				Entities.Remove(Entity);
				NotifyEntityRemoved(Entity);
				delete Entity;
			}
		}
	}
}

void FWorld::NotifyEntityAdded(FEntity* Entity)
{
	for (auto Sets : EntitySets)
	{
		Sets->OnEntityAdded(Entity);
	}
}

void FWorld::NotifyEntityRemoved(FEntity* Entity)
{
	for (auto Sets : EntitySets)
	{
		Sets->OnEntityRemoved(Entity);
	}
}

void FWorld::NotifyEntityComponentsChanged(FEntity* Entity)
{
	for (auto Sets : EntitySets)
	{
		Sets->OnComponentsChanged(Entity);
	}
}

FEntitySet* FWorld::GetEntitySet(const FClassList& ComponentsSet)
{
	FEntitySet* EntitySet = nullptr;

	auto Position = std::find_if(EntitySets.begin(), EntitySets.end(),
		[&] (const FEntitySet* Set) { return Set->GetRequiredComponents() == ComponentsSet; });

	if (Position == EntitySets.end())
	{
		EntitySet = new FEntitySet(ComponentsSet);
		EntitySets.Add(EntitySet);
		
		for (auto Entity : Entities)
		{
			EntitySet->OnEntityAdded(Entity);
		}
	}
	else
	{
		EntitySet = (*Position);
	}

	return EntitySet;
}

FEntity* FWorld::GetEntityByName(const std::string& Name)
{
	auto Position = std::find_if(Entities.begin(), Entities.end(),
		[&](FEntity* Entity)
	{
		return Entity->GetName() == Name;
	});

	if (Position == Entities.end())
		return nullptr;

	return (*Position);
}

void FWorld::Destroy()
{
	FProfiler::Get().ResetCounter("Gameplay", "Entity Count", false);
	while (Entities.Size() > 0)
	{
		Entities[0]->Kill();
		CheckPendingEntities();
	}
}

