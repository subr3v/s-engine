#include "System.h"
#include "World.h"

FGameSystem::FGameSystem(const FClassInfo* ClassInfo) : ClassInfo(ClassInfo)
{

}

FGameSystem::~FGameSystem()
{

}

FEntitySet* FGameSystem::GetEntitySet(std::initializer_list<const FClassInfo*> Classes)
{
	return World->GetEntitySet(Classes);
}

void FGameSystem::OnAdded()
{

}

void FGameSystem::OnRemoved()
{
	
}

