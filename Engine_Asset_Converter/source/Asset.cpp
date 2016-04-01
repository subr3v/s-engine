#include "Asset.h"
#include "CoreTypes.h"
#include <algorithm>
#include "Utils.h"

FAsset::FAsset(const std::string& Filename) : Filename(Filename)
{
	SizeType LastIndex = Filename.find_last_of('.');
	if (LastIndex >= 0)
	{
		Extension = Filename.substr(LastIndex);
	}
	Name = RemoveExtension(GetNameFromPath(Filename));
	FilenameNoExtension = RemoveExtension(Filename);
}

FAsset::~FAsset()
{

}
