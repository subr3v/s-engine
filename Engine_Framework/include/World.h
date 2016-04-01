#ifndef World_h__
#define World_h__

#include <vector>
#include <algorithm>
#include <initializer_list>
#include "ClassType.h"
#include "ClassList.h"

class FEntity;
class FGameSystem;
class FEntitySet;
class FClassInfo;
class FComponent;

class FWorld
{
public:
	FWorld();
	~FWorld();

	FEntity* CreateEntity();
	FEntity* CreateEntity(std::initializer_list<FComponent*> Components);
	// Note: this is a linear search so use it sparingly.
	FEntity* GetEntityByName(const std::string& Name);

	void Update(float DeltaTime);
	void Render();

	// Note: these are not meant to be added or removed during game execution, ie: bad things would happen if you try to
	// add or remove these within other systems logic callbacks.
	void AddSystem(FGameSystem* GameSystem);
	void RemoveSystem(FGameSystem* GameSystem);

	template <typename T>
	T* GetSystem() const
	{
		const FClassInfo* ClassInfo = TYPE_OF(T);
		auto Position = std::find_if(Systems.begin(), Systems.end(),
			[&] (const FGameSystem* System) 
				{ return System->GetClassInfo() == ClassInfo; }
		);
		if (Position != Systems.end())
			return (T*)(*Position);
		return nullptr;
	}

	FEntitySet* GetEntitySet(const FClassList& ComponentsSet);

	template <class Predicate>
	FEntity* Find(Predicate Pr) const
	{
		auto Position = std::find_if(Entities.begin(), Entities.end(), Pr);
		if (Position == Entities.end())
			return nullptr;
		return (*Position);
	}

	void Destroy();
private:
	void PreUpdate(float DeltaTime);
	void OnUpdate(float DeltaTime);
	void PostUpdate(float DeltaTime);

	void PreRender();
	void OnRender();
	void PostRender();

	void CheckPendingEntities();

	void NotifyEntityAdded(FEntity* Entity);
	void NotifyEntityRemoved(FEntity* Entity);
	void NotifyEntityComponentsChanged(FEntity* Entity);

	TVector<FGameSystem*> Systems;
	TVector<FEntity*> Entities;
	TVector<FEntitySet*> EntitySets;

	friend class FEntity;
};

#endif // World_h__
