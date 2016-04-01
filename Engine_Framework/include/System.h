#ifndef System_h__
#define System_h__

#include <vector>
#include "Core.hpp"

class FWorld;
class FEntitySet;

class FGameSystem
{
public:
	FGameSystem(const FClassInfo* ClassInfo);
	virtual ~FGameSystem();

	virtual void OnAdded();
	virtual void OnRemoved();

	virtual void PreUpdate(float DeltaTime) { };
	virtual void Update(float DeltaTime) { };
	virtual void PostUpdate(float DeltaTime) { };

	virtual void PreRender() { };
	virtual void Render() { };
	virtual void PostRender() { };

	const FClassInfo* GetClassInfo() const { return ClassInfo; }

protected:
	FEntitySet* GetEntitySet(std::initializer_list<const FClassInfo*> Classes);
	const FWorld* GetWorld() const { return World; }
	FWorld* GetWorld() { return World; }
private:
	const FClassInfo* ClassInfo;
	FWorld* World;
	friend class FWorld;
};

template <typename T>
class TGameSystem : public FGameSystem
{
public:
	TGameSystem() : FGameSystem(TYPE_OF(T)) { }
};

#endif // System_h__
