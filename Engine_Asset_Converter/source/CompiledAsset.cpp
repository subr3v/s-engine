#include "CompiledAsset.h"
#include "zip.h"
#include "ioapi_mem.h"
#include "unzip.h"

FCompiledAsset::FCompiledAsset(FAsset Asset, const std::string& AssetType) : SourceAsset(Asset), AssetType(AssetType)
{

}
