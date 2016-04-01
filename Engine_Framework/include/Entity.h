#ifndef Entity_h__
#define Entity_h__

#include "Core.hpp"
#include <map>

class FComponent;
class FWorld;
class FClassInfo;

class FEntity
{
public:
	FComponent* AddComponent(FComponent* Component);
	bool RemoveComponent(FComponent* Component);

	FComponent* GetComponent(const FClassInfo* ClassInfo);
	bool HasComponent(const FClassInfo* ClassInfo);

	template <typename T>
	T* GetComponent()
	{
		return (T*)GetComponent(TClassTypeId<T>::Get());
	}

	const std::map<const FClassInfo*, FComponent*>& GetComponentsMap() const { return Components; }

	void SetName(const std::string& Name);
	const std::string& GetName() const { return Name; }

	void Kill();
private:
	FEntity(FWorld* World);
	~FEntity();

	friend class FWorld;

	FWorld* World;
	bool bMarkedForDestruction;
	std::string Name;

	std::map<const FClassInfo*, FComponent*> Components;
};

#endif // Entity_h__
