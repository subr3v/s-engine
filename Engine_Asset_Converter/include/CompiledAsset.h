#ifndef CompiledAsset_h__
#define CompiledAsset_h__

#include <vector>
#include "Core.hpp"
#include "Asset.h"
#include <string>

class FCompiledAsset
{
public:
	FCompiledAsset(FAsset Asset, const std::string& AssetType);
	~FCompiledAsset() { }

	const FAsset& GetSourceAsset() const { return SourceAsset; }
	const std::string& GetType() const { return AssetType; }
private:
	FAsset SourceAsset;
	std::string AssetType;
};

#endif // CompiledAsset_h__
