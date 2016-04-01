#include "IniFile.h"
#include <fstream>

static std::string StripCategoryToken(std::string Line)
{
	return Line.substr(1, Line.length() - 2);
}

bool FIni::GetSection(const std::string& Name, FIniSection** OutSection, bool bCreateIfNotExists)
{
	auto Result = Data.find(Name);
	if (Result == Data.end() && bCreateIfNotExists)
	{
		Data[Name] = FIniSection();
	}
	else
	{
		return false;
	}

	*OutSection = &Data[Name];
	return true;
}

void FIni::LoadFromFile(const std::string& Filename)
{
	std::ifstream File(Filename);

	if (File.is_open() == false)
		return;

	std::string CategoryName;
	File >> CategoryName;

	FIniSection* CurrentSection;
	GetSection(StripCategoryToken(CategoryName), &CurrentSection, true);

	while (File.eof() == false)
	{
		std::string Line;
		std::getline(File, Line);

		if (Line.length() <= 0)
			continue;

		if (Line[0] == '[')
		{
			// New category.
			GetSection(StripCategoryToken(Line), &CurrentSection, true);
		}
		else
		{
			// New pair
			std::size_t TokenPosition = Line.find('=');
			std::string Key = Line.substr(0, TokenPosition - 1);
			std::string Value = Line.substr(TokenPosition + 2);
			(*CurrentSection)[Key] = Value;
		}
	}
}

void FIni::SaveToFile(const std::string& Filename)
{
	std::ofstream File(Filename);
	for (auto Category : Data)
	{
		File << "[" << Category.first << "]" << std::endl;
		for (auto Pair : Category.second)
		{
			File << Pair.first << " = " << Pair.second << std::endl;
		}
		File << std::endl;
	}
}

FIni::DataType& FIni::GetData()
{
	return Data;
}

