#include "MaterialsConfiguration.h"
#include <fstream>
#include <locale>
#include <sstream>
#include <algorithm>

void TrimString(std::string& String)
{
	String.erase(std::remove(String.begin(), String.end(), ' '), String.end());
}

void FMaterialConfiguration::LoadFromFile(const std::string& Path)
{
	std::fstream Filestream(Path);

	typedef std::codecvt<wchar_t, char, _Mbstatet> convert_type;
	std::wstring_convert<convert_type, wchar_t> converter;

	bool EndOfFile = false;

	while (EndOfFile == false && Filestream.is_open())
	{
		std::string Line;
		std::getline(Filestream, Line);

		std::stringstream LineStream(Line);

		std::string Path;

		FMaterialDescriptor Descriptor;
		std::getline(LineStream, Descriptor.Name, ',');
		std::getline(LineStream, Path, ',');

		TrimString(Descriptor.Name);
		TrimString(Path);

		Descriptor.Path = converter.from_bytes(Path);

		if (LineStream.eof() == false)
		{
			while (LineStream.eof() == false)
			{
				std::string Define;
				std::getline(LineStream, Define, ',');
				TrimString(Define);
				Descriptor.Defines.Add(Define);
			}
		}

		Descriptors.Add(Descriptor);

		EndOfFile = Filestream.eof();
	}
}
