#ifndef Component_h__
#define Component_h__

#include "Core.hpp"

/*!
 * \class FComponent
 *
 * \brief This class is meant to be a base class for any component within the entity system
 * A component should only hold data if possible, but utility methods are allowed.
 *
 * \author Stefano
 * \date December 2015
 */
class FComponent
{
public:
	FComponent(const FClassInfo* ClassInfo = nullptr);
	virtual ~FComponent();

	const FClassInfo* GetClassInfo() const { return ClassInfo; }
private:
	const FClassInfo* ClassInfo;
};

template <typename T>
class TComponent : public FComponent
{
public:
	TComponent() : FComponent(TClassTypeId<T>::Get()) 
	{
		FProfiler::Get().ProfileCounter("Gameplay", TClassTypeId<T>::Get()->GetPrintName().c_str(), false, 1);
	}

	virtual ~TComponent() 
	{
		FProfiler::Get().ProfileCounter("Gameplay", TClassTypeId<T>::Get()->GetPrintName().c_str(), false, -1);
	}
};

#endif // Component_h__
