#ifndef ResourceContainer_h__
#define ResourceContainer_h__

#include "Vector.h"
#include <string>
#include <cassert>
#include <cstdint>

template <class ResourceType>
class TResourceContainer
{
public:

	~TResourceContainer();

	void AddResource(const std::string& Name, ResourceType* Resource);
	void RemoveResource(const std::string& Name);
	ResourceType* GetResource(ConstantString Name) const;
	ResourceType* GetResource(const std::string& Name) const;
	void UnloadResources();

	uint32_t GetResourcesCount() const { return Resources.Size(); }
	ResourceType* GetResourceAtIndex(int32_t Index) const { return Resources[Index].Resource; }

	class iterator
	{
	public:
		iterator() { }

		iterator& operator++()
		{
			CurrentIndex++;
			return *this;
		}

		bool operator==(const iterator& Other) const
		{
			return Container == Other.Container && CurrentIndex == Other.CurrentIndex;
		}

		bool operator!=(const iterator& Other) const
		{
			return !(*this == Other);
		}

		ResourceType* operator*()
		{
			return Container->GetResourceAtIndex(CurrentIndex);
		}

		ResourceType* operator->()
		{
			return Container->GetResourceAtIndex(CurrentIndex);
		}

		static iterator Create(TResourceContainer<ResourceType>* Container, int32_t Index)
		{
			iterator it;
			it.Container = Container;
			it.CurrentIndex = Index;
			return it;
		}

	private:
		TResourceContainer<ResourceType>* Container;
		int32_t CurrentIndex;
	};

	iterator begin()
	{
		return iterator::Create(this, 0);
	}

	iterator end()
	{
		return iterator::Create(this, Resources.Size());
	}

private:
	struct ResourceView
	{
		std::string ResourceName;
		ResourceType* Resource;

		ResourceView() { }
		ResourceView(const std::string& Name, ResourceType* Resource) : ResourceName(Name), Resource(Resource) { }
	};
	TVector<ResourceView> Resources;
	typedef typename TVector<ResourceView>::Iterator ResourceIterator;
};

template <class ResourceType>
void TResourceContainer<ResourceType>::UnloadResources()
{
	for (auto& Resource : Resources)
	{
		delete Resource.Resource;
	}

	Resources.Clear();
}

template <class ResourceType>
void TResourceContainer<ResourceType>::AddResource(const std::string& Name, ResourceType* Resource)
{
	if (GetResource(Name) != nullptr)
	{
		RemoveResource(Name);
		// assert(false); // TODO: find a less brutal way.
	}
	Resources.Add(ResourceView(Name, Resource));
}

template <class ResourceType>
ResourceType* TResourceContainer<ResourceType>::GetResource(ConstantString Name) const
{
	for (auto& Resource : Resources)
	{
		if (strcmp(Resource.ResourceName.c_str(), Name) == 0)
		{
			return Resource.Resource;
		}
	}

	return nullptr;
}


template <class ResourceType>
ResourceType* TResourceContainer<ResourceType>::GetResource(const std::string& Name) const
{
	for (auto& Resource : Resources)
	{
		if (Resource.ResourceName == Name)
		{
			return Resource.Resource;
		}
	}

	return nullptr;
}

template <class ResourceType>
void TResourceContainer<ResourceType>::RemoveResource(const std::string& Name)
{
	for (ResourceIterator it = Resources.begin(); it != Resources.end(); ++it)
	{
		if ((*it).ResourceName == Name)
		{
			Resources.RemoveAt(it);
			break;
		}
	}
}

template <class ResourceType>
TResourceContainer<ResourceType>::~TResourceContainer()
{
	UnloadResources();
}

#endif // ResourceContainer_h__
