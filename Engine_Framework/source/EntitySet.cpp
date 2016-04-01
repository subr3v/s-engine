#include "EntitySet.h"

#include <algorithm>
#include "Entity.h"
#include "ClassList.h"

FEntitySet::FEntitySet(const FClassList& ClassList) : RequiredComponents(ClassList)
{

}

FEntitySet::~FEntitySet()
{
	Entities.Clear();
}

void FEntitySet::OnEntityAdded(FEntity* Entity)
{
	if (IsEntityValid(Entity))
	{
		Entities.Add(Entity);
	}
}

void FEntitySet::OnEntityRemoved(FEntity* Entity)
{
	Entities.Remove(Entity);
}

void FEntitySet::OnComponentsChanged(FEntity* Entity)
{
	if (IsEntityValid(Entity))
	{
		// Only add if it's not present already.
		auto Position = std::find(Entities.begin(), Entities.end(), Entity);
		if (Position == Entities.end())
		{
			Entities.Add(Entity);
		}
	}
	else
	{
		// Only remove if it's present.
		Entities.Remove(Entity);
	}
}

bool FEntitySet::IsEntityValid(FEntity* Entity)
{
	if (Entity == nullptr)
	{
		return false;
	}

	int32 Count = 0;
	for (auto Pair : Entity->GetComponentsMap())
	{
		if (RequiredComponents.HasClass(Pair.first))
		{
			Count++;
		}
	}

	return Count == RequiredComponents.GetCount();
}

const TVector<FEntity*>& FEntitySet::Get() const
{
	return Entities;
}

const FClassList& FEntitySet::GetRequiredComponents() const
{
	return RequiredComponents;
}
