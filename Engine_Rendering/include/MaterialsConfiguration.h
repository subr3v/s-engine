#ifndef MaterialsConfiguration_h__
#define MaterialsConfiguration_h__

#include "Vector.h"
#include <string>

typedef TVector<std::string> DefinesVector;

struct FMaterialDescriptor
{
	std::string Name;
	std::wstring Path;
	DefinesVector Defines;

	FMaterialDescriptor() { }
	FMaterialDescriptor(const std::string& Name, const std::wstring& Path, const DefinesVector& Defines = DefinesVector()) : Name(Name), Path(Path), Defines(Defines) { }
};

class FMaterialConfiguration
{
public:

	void LoadFromFile(const std::string& Path);
	const TVector<FMaterialDescriptor>& GetDescriptors() const
	{
		return Descriptors;
	}
private:
	TVector<FMaterialDescriptor> Descriptors;
};

#endif // MaterialsConfiguration_h__
