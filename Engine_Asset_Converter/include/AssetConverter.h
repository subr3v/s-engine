#ifndef AssetConverter_h__
#define AssetConverter_h__

#include "Serializer.h"
#include "Asset.h"

typedef bool(*IsExtensionSupportedPtr)(const std::string&);
typedef const char*(*GetTypeNamePtr)(void);
typedef bool(*ConvertAssetPtr)(const FAsset& Asset, FSerializer& Serializer);

class FAssetConverter
{
public:
	FAssetConverter(const std::string& Filename);
	~FAssetConverter();

	bool IsExtensionSupported(const std::string& Extension);
	bool ConvertAsset(const FAsset& Asset, FSerializer& Serializer);
	std::string GetTypeName();

private:
	HINSTANCE LibraryModule;
	IsExtensionSupportedPtr IsExtensionSupportedImpl;
	ConvertAssetPtr ConvertAssetImpl;
	GetTypeNamePtr GetTypeNameImpl;
};

#endif // AssetConverter_h__
 