#ifndef ResourcePackage_h__
#define ResourcePackage_h__

#include "Vector.h"
#include <string>
#include <map>
#include "Serializer.h"
#include "ZipArchive.h"

class FResource;

class FResourcePackage
{
public:
	FResourcePackage(const std::string& Path);
	~FResourcePackage();

	FResource* GetResource(const std::string& Name) const;
	const TVector<FResource>& GetResources() const;

	FZipArchive* GetArchive();
private:
	std::map<std::string, FResource*> ResourcesMap;
	TVector<FResource> Resources;
	FZipArchive Archive;
};

class FResource
{
public:
	FResource(const std::string& Name, const std::string& Type, const std::string& Path);
	FResource();

	const std::string& GetName() const { return Name; }
	const std::string& GetType() const { return Type; }
	const std::string& GetPath() const { return Path; }
	// Note: you can only open one archive at a time.
	IArchive* GetResourceArchive(FResourcePackage* Package);
	void Serialize(FSerializer& Serializer);
private:
	std::string Name;
	std::string Type;
	std::string Path;
};

struct FResourcePackageDescription
{
	TVector<FResource> Resources;

	static const std::string kDescriptionSubFileName;
	void Serialize(FSerializer& Serializer);
};

#endif // ResourcePackage_h__
