#ifndef ConfigVariableManager_h__
#define ConfigVariableManager_h__

#include "Vector.h"
#include <string>

class FConfigVariable;

struct FConfigVariableCategory
{
	FConfigVariableCategory();
	FConfigVariableCategory(const std::string& Name);

	FConfigVariable* GetVariableByName(const std::string& Name);

	const TVector<FConfigVariable*>& GetVariables() const;

	ConstantString GetName() const;
private:
	friend class FConfigVariableManager;
	std::string Name;
	TVector<FConfigVariable*> Variables;
};

class FConfigVariableManager
{
public:
	static FConfigVariableManager& Get();

	void Initialise();
	void Shutdown();

	void Load();
	void Save();

	const TVector<FConfigVariableCategory>& GetCategories() const;
	FConfigVariableCategory* GetVariablesCategory(const std::string& Category, bool bCreateIfNotExisting = false);
private:
	std::string GetConfigurationFileName() const;
	FConfigVariableManager();

	friend class FConfigVariable;

	void RegisterVariable(FConfigVariable* ConfigVariable);
	TVector<FConfigVariableCategory> Categories;
};

#endif // ConfigVariableManager_h__
