#include "AssetCompiler.h"
#include <algorithm>
#include "Serializer.h"
#include "MemoryArchive.h"
#include "ZipArchive.h"

FAssetCompiler::FAssetCompiler()
{

}

FAssetCompiler::~FAssetCompiler()
{
	std::for_each(Converters.begin(), Converters.end(), 
		[](FAssetConverter* Converter)
	{
		delete Converter;
	});
	Converters.Clear();;
}

void FAssetCompiler::RegisterAssetConverter(FAssetConverter* Converter)
{
	Converters.Add(Converter);
}

FCompiledAsset* FAssetCompiler::CompileAsset(const FAsset& Asset, IArchive* Archive)
{
	ConverterList::Iterator Converter = std::find_if(Converters.begin(), Converters.end(),
		[Asset](FAssetConverter* Converter)
	{
		return Converter->IsExtensionSupported(Asset.GetExtension());
	});

	if (Converter != Converters.end())
	{
		return GenerateCompiledAsset((*Converter), Asset, Archive);
	}

	// Trace some info about not having found a compatible converter

	return nullptr;
}

FCompiledAsset* FAssetCompiler::GenerateCompiledAsset(FAssetConverter* Converter, const FAsset& Asset, IArchive* Archive)
{
	FSerializer Serializer(Archive, false, false);
	if (Converter->ConvertAsset(Asset, Serializer))
	{
		FCompiledAsset* CompiledAsset = new FCompiledAsset(Asset, Converter->GetTypeName());
		return CompiledAsset;
	}

	return nullptr;
}
