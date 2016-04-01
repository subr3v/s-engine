#include "ResourcePackage.h"

const std::string FResourcePackageDescription::kDescriptionSubFileName = "header.pak";

FResource::FResource()
{

}

FResource::FResource(const std::string& Name, const std::string& Type, const std::string& Path) : Name(Name), Type(Type), Path(Path)
{

}

void FResource::Serialize(FSerializer& Serializer)
{
	Serializer.SerializeString(Name);
	Serializer.SerializeString(Type);
	Serializer.SerializeString(Path);
}

IArchive* FResource::GetResourceArchive(FResourcePackage* Package)
{
	return Package->GetArchive()->OpenSubFileArchive(Path);
}

FResourcePackage::FResourcePackage(const std::string& Path) : Archive(EZipArchiveMode::Read)
{
	Archive.OpenFile(Path);
	bool bDescriptionExists = Archive.OpenSubFile(FResourcePackageDescription::kDescriptionSubFileName);
	if (bDescriptionExists)
	{
		FSerializer Serializer(&Archive, true, false);
		FResourcePackageDescription PackageDescription;
		PackageDescription.Serialize(Serializer);
		Archive.CloseSubFile();

		Resources = PackageDescription.Resources;
		for (FResource& Resource : Resources)
		{
			ResourcesMap[Resource.GetName()] = &Resource;
		}
	}
}

FResourcePackage::~FResourcePackage()
{
	Archive.Close();
}

FResource* FResourcePackage::GetResource(const std::string& Name) const
{
	auto Result = ResourcesMap.find(Name);
	if (Result == ResourcesMap.end())
	{
		return nullptr;
	}
	return (*Result).second;
}

const TVector<FResource>& FResourcePackage::GetResources() const
{
	return Resources;
}

FZipArchive* FResourcePackage::GetArchive()
{
	return &Archive;
}

void FResourcePackageDescription::Serialize(FSerializer& Serializer)
{
	Serializer.SerializeObjectArray(Resources);
}
