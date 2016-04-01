#ifndef AssetCompiler_h__
#define AssetCompiler_h__

#include "Asset.h"
#include "CompiledAsset.h"
#include "AssetConverter.h"
#include "Vector.h"

class FAssetCompiler
{
public:
	FAssetCompiler();
	~FAssetCompiler();

	void RegisterAssetConverter(FAssetConverter* Converter);
	FCompiledAsset* CompileAsset(const FAsset& Asset, IArchive* Archive);
private:
	FCompiledAsset* GenerateCompiledAsset(FAssetConverter* Converter, const FAsset& Asset, IArchive* Archive);

	typedef TVector<FAssetConverter*> ConverterList;

	ConverterList Converters;
};

#endif // AssetCompiler_h__
