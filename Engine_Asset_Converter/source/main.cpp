#include "AssetCompiler.h"
#include <conio.h>
#include <string>
#include <iostream>
#include <fstream>
#include "ZipArchive.h"
#include "ResourcePackage.h"

int main(int argc, char** argv)
{
	FAssetCompiler AssetCompiler;
	AssetCompiler.RegisterAssetConverter(new FAssetConverter("Asset_Texture_Loader.dll"));
	AssetCompiler.RegisterAssetConverter(new FAssetConverter("Asset_Model_Loader.dll"));

	FResourcePackageDescription PackageDescription;
	FZipArchive PakFile(EZipArchiveMode::Write);
	PakFile.OpenFile("Result.pak");

	for (int i = 1; i < argc; i++)
	{
		FAsset Asset(argv[i]);
		std::string TargetFilename = Asset.GetFilenameNoExtension() + ".dat";
		std::string AssetPath = Asset.GetName() + Asset.GetExtension();
		PakFile.OpenSubFile(AssetPath);
		FCompiledAsset* CompiledAsset = AssetCompiler.CompileAsset(Asset, &PakFile);
		PakFile.CloseSubFile();
		if (CompiledAsset != nullptr)
		{
			PackageDescription.Resources.Add(FResource(Asset.GetName(), CompiledAsset->GetType(), AssetPath));
			std::cout << "Converted file successfully: " << AssetPath << std::endl;
		}
		else
			std::cout << "Couldn't find a suitable converter for: " << Asset.GetFilename() << std::endl;
	}

	PakFile.OpenSubFile(FResourcePackageDescription::kDescriptionSubFileName);
	FSerializer DescriptionSerializer(&PakFile, false, false);
	PackageDescription.Serialize(DescriptionSerializer);
	PakFile.CloseSubFile();
	PakFile.Close();

	FResourcePackage TestPackage("Result.pak");

	_getch();
}