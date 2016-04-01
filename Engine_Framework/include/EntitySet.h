#ifndef EntitySet_h__
#define EntitySet_h__

#include <vector>
#include <algorithm>
#include "ClassType.h"
#include "ClassList.h"

class FClassInfo;
class FEntity;

class FEntitySet 
{
public:
	FEntitySet(const FClassList& ClassList);
	~FEntitySet();

	const TVector<FEntity*>& Get() const;
	const FClassList& GetRequiredComponents() const;

	template <class Predicate>
	FEntity* Find(Predicate Pred) const
	{
		auto Position = std::find_if(Entities.begin(), Entities.end(), Pred);
		return (Position == Entities.end()) ? nullptr : (*Position);
	}
private:
	friend class FWorld;

	void OnEntityAdded(FEntity* Entity);
	void OnEntityRemoved(FEntity* Entity);
	void OnComponentsChanged(FEntity* Entity);

	bool IsEntityValid(FEntity* Entity);

	FClassList RequiredComponents;
	TVector<FEntity*> Entities;
};

#endif // EntitySet_h__
