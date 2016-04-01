#include "ConfigVariableManager.h"
#include "ConfigVariable.h"
#include "IniFile.h"
#include <Windows.h>
#include "Utils.h"

ConstantString FConfigVariableCategory::GetName() const
{
	return Name.c_str();
}

FConfigVariableCategory::FConfigVariableCategory(const std::string& Name) : Name(Name)
{

}

struct FFindCategoryByName
{
	FFindCategoryByName(const std::string& Name) : Name(Name) { }

	bool operator ()(FConfigVariableCategory& Category)
	{
		return strcmp(Category.GetName(), Name.c_str()) == 0;
	}

	const std::string& Name;
};

struct FFindVariableByName
{
	FFindVariableByName(const std::string& Name) : Name(Name) { }

	bool operator ()(FConfigVariable* Variable)
	{
		return strcmp(Variable->GetName(), Name.c_str()) == 0;
	}

	const std::string& Name;
};

FConfigVariableManager& FConfigVariableManager::Get()
{
	static FConfigVariableManager Manager;
	return Manager;
}

FConfigVariableManager::FConfigVariableManager()
{

}

void FConfigVariableManager::Initialise()
{
	Load();
}

void FConfigVariableManager::Shutdown()
{
	Save();
	Categories.Clear();
}

void FConfigVariableManager::RegisterVariable(FConfigVariable* ConfigVariable)
{
	FConfigVariableCategory* Category = GetVariablesCategory(ConfigVariable->GetCategory(), true);
	Category->Variables.Add(ConfigVariable);
}

FConfigVariableCategory* FConfigVariableManager::GetVariablesCategory(const std::string& CategoryName, bool bCreateIfNotExisting)
{
	FFindCategoryByName SearchFunctor(CategoryName);
	FConfigVariableCategory* Result = Categories.Find(SearchFunctor);
	if (Result == nullptr && bCreateIfNotExisting)
	{
		FConfigVariableCategory Category(CategoryName);
		Categories.Add(Category);
		return &Categories.Last();
	}
	return Result;
}

const TVector<FConfigVariableCategory>& FConfigVariableManager::GetCategories() const
{
	return Categories;
}

void FConfigVariableManager::Load()
{
	// Get name from current executable:
	FIni ConfigFile;
	ConfigFile.LoadFromFile(GetConfigurationFileName());
	FIni::DataType& ConfigurationData = ConfigFile.GetData();
	for (auto Pair : ConfigurationData)
	{
		FConfigVariableCategory* Category = GetVariablesCategory(Pair.first);
		if (Category != nullptr)
		{
			for (auto KeyValue : Pair.second)
			{
				FConfigVariable* Variable = Category->GetVariableByName(KeyValue.first);
				if (Variable != nullptr)
				{
					Variable->FromString(KeyValue.second);
				}
			}
		}
	}
}

void FConfigVariableManager::Save()
{
	FIni ConfigFile;

	for (FConfigVariableCategory& Category : Categories)
	{
		FIniSection* Section;
		ConfigFile.GetSection(Category.GetName(), &Section, true);
		for (FConfigVariable* Variable : Category.Variables)
		{
			(*Section)[Variable->GetName()] = Variable->ToString();
		}
	}

	ConfigFile.SaveToFile(GetConfigurationFileName());
}

std::string FConfigVariableManager::GetConfigurationFileName() const
{
	char FileNameBuffer[256];
	GetModuleFileName(nullptr, FileNameBuffer, sizeof(FileNameBuffer));
	std::string ConfigurationFileName = RemoveExtension(GetNameFromPath(FileNameBuffer));
	return ConfigurationFileName + ".ini";
}

FConfigVariable* FConfigVariableCategory::GetVariableByName(const std::string& Name)
{
	FFindVariableByName SearchFunctor(Name);
	auto Result = Variables.Find(SearchFunctor);
	if (Result != nullptr)
	{
		return *Result;
	}
	return nullptr;
}

const TVector<FConfigVariable*>& FConfigVariableCategory::GetVariables() const
{
	return Variables;
}
