#ifndef TextureConverter_h__
#define TextureConverter_h__

#include "CoreTypes.h"
#include <string>
#include "Serializer.h"
#include "Asset.h"

extern "C"
{
	ENGINE_EXPORT_DLL bool IsExtensionSupported(const std::string& Extension);
	ENGINE_EXPORT_DLL bool ConvertAsset(const FAsset& Asset, FSerializer& Serializer);
	ENGINE_EXPORT_DLL const char* GetTypeName();
}

#endif // TextureConverter_h__
