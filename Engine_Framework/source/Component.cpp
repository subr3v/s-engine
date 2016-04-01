#include "Component.h"

FComponent::FComponent(const FClassInfo* ClassInfo) : ClassInfo(ClassInfo)
{

}

FComponent::~FComponent()
{
	ClassInfo = nullptr;
}

