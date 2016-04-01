#include "AssetConverter.h"

static const char* IsExtensionSupportedProcName = "IsExtensionSupported";
static const char* ConvertAssetProcName = "ConvertAsset";
static const char* GetTypeNameProcName = "GetTypeName";

FAssetConverter::FAssetConverter(const std::string& Filename)
{
	LibraryModule = LoadLibrary(Filename.c_str());

	IsExtensionSupportedImpl = (IsExtensionSupportedPtr)GetProcAddress(LibraryModule, IsExtensionSupportedProcName);
	ConvertAssetImpl = (ConvertAssetPtr)GetProcAddress(LibraryModule, ConvertAssetProcName);
	GetTypeNameImpl = (GetTypeNamePtr)GetProcAddress(LibraryModule, GetTypeNameProcName);

	assert(LibraryModule);
	assert(IsExtensionSupportedImpl);
	assert(ConvertAssetImpl);
	assert(GetTypeNameImpl);
}

FAssetConverter::~FAssetConverter()
{
	FreeLibrary(LibraryModule);
}

bool FAssetConverter::IsExtensionSupported(const std::string& Extension)
{
	if (IsExtensionSupportedImpl)
	{
		return IsExtensionSupportedImpl(Extension);
	}
	return false;
}

bool FAssetConverter::ConvertAsset(const FAsset& Asset, FSerializer& Serializer)
{
	if (ConvertAssetImpl)
	{
		return ConvertAssetImpl(Asset, Serializer);
	}
	return false;
}

std::string FAssetConverter::GetTypeName()
{
	if (GetTypeNameImpl)
	{
		return GetTypeNameImpl();
	}
	return "Error";
}
