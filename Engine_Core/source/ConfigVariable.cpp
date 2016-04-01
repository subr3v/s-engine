#include "ConfigVariable.h"
#include "ConfigVariableManager.h"

FConfigVariable::FConfigVariable(ConstantString Category, ConstantString Name, bool DefaultValue) : Category(Category), Name(Name), VariableType(EConfigVariableType::Boolean)
{
	CurrentVariable.AsBoolean = DefaultValue;
	FConfigVariableManager::Get().RegisterVariable(this);
}

FConfigVariable::FConfigVariable(ConstantString Category, ConstantString Name, int32 DefaultValue) : Category(Category), Name(Name), VariableType(EConfigVariableType::Integer)
{
	CurrentVariable.AsInt = DefaultValue;
	FConfigVariableManager::Get().RegisterVariable(this);
}

FConfigVariable::FConfigVariable(ConstantString Category, ConstantString Name, float DefaultValue) : Category(Category), Name(Name), VariableType(EConfigVariableType::Float)
{
	CurrentVariable.AsFloat = DefaultValue;
	FConfigVariableManager::Get().RegisterVariable(this);
}

FConfigVariable::FConfigVariable(ConstantString Category, ConstantString Name, const std::string& DefaultValue) : Category(Category), Name(Name), VariableType(EConfigVariableType::String)
{
	StringVar = DefaultValue;
	StringVar.reserve(kStringMaxLength);
	FConfigVariableManager::Get().RegisterVariable(this);
}

FConfigVariable::FConfigVariable(ConstantString Category, ConstantString Name, const char* DefaultValue) : FConfigVariable(Category, Name, std::string(DefaultValue))
{

}

FConfigVariable::FConfigVariable(ConstantString Category, ConstantString Name, glm::vec2 DefaultValue) : Category(Category), Name(Name), VariableType(EConfigVariableType::Vector2)
{
	AsVec2() = DefaultValue;
	FConfigVariableManager::Get().RegisterVariable(this);
}

FConfigVariable::FConfigVariable(ConstantString Category, ConstantString Name, glm::vec3 DefaultValue) : Category(Category), Name(Name), VariableType(EConfigVariableType::Vector3)
{
	AsVec3() = DefaultValue;
	FConfigVariableManager::Get().RegisterVariable(this);
}

EConfigVariableType FConfigVariable::GetType() const
{
	return VariableType;
}

bool& FConfigVariable::AsBoolean()
{
	assert(VariableType == EConfigVariableType::Boolean);
	return CurrentVariable.AsBoolean;
}

int32& FConfigVariable::AsInt()
{
	assert(VariableType == EConfigVariableType::Integer);
	return CurrentVariable.AsInt;
}

float& FConfigVariable::AsFloat()
{
	assert(VariableType == EConfigVariableType::Float);
	return CurrentVariable.AsFloat;
}

std::string& FConfigVariable::AsString()
{
	assert(VariableType == EConfigVariableType::String);
	return StringVar;
}

// I am writing those very "wrong and dangerous" casts here: this allows the user to manipulate these vectors
// as if they were instances of glm::vec2, the reason is I can't simply have them as glm::vec2/3 within the union
// because they have non trivial constructors when they actually behave as simple float vectors inside.
glm::vec2& FConfigVariable::AsVec2()
{
	assert(VariableType == EConfigVariableType::Vector2);
	return *((glm::vec2*)CurrentVariable.AsVec2);
}

glm::vec3& FConfigVariable::AsVec3()
{
	assert(VariableType == EConfigVariableType::Vector3);
	return *((glm::vec3*)CurrentVariable.AsVec3);
}

ConstantString FConfigVariable::GetName() const
{
	return Name;
}

ConstantString FConfigVariable::GetCategory() const
{
	return Category;
}

void FConfigVariable::FromString(const std::string& Text)
{
	std::size_t TokenIndex;
	switch (VariableType)
	{
	case EConfigVariableType::Float:
		CurrentVariable.AsFloat = std::stof(Text);
		break;
	case EConfigVariableType::Boolean:
	case EConfigVariableType::Integer:
		CurrentVariable.AsInt = std::stoi(Text);
		break;
	case EConfigVariableType::String:
		StringVar = Text;
		break;
	case EConfigVariableType::Vector2:
		TokenIndex = Text.find(',');
		CurrentVariable.AsVec2[0] = std::stof(Text.substr(2, TokenIndex - 1));
		CurrentVariable.AsVec2[1] = std::stof(Text.substr(TokenIndex + 1, Text.length() - TokenIndex - 1));
		break;
	case EConfigVariableType::Vector3:
		TokenIndex = Text.find(',');
		CurrentVariable.AsVec3[0] = std::stof(Text.substr(2, TokenIndex - 1));
		CurrentVariable.AsVec3[1] = std::stof(Text.substr(TokenIndex + 1, Text.length() - TokenIndex - 1));
		TokenIndex = Text.find(',', TokenIndex + 1);
		CurrentVariable.AsVec3[2] = std::stof(Text.substr(TokenIndex + 1, Text.length() - TokenIndex - 1));
		break;
	}
}

std::string FConfigVariable::ToString() const
{
	switch (VariableType)
	{
		case EConfigVariableType::Float:
			return std::to_string(CurrentVariable.AsFloat);
		case EConfigVariableType::Boolean:
		case EConfigVariableType::Integer:
			return std::to_string(CurrentVariable.AsInt);
		case EConfigVariableType::String:
			return StringVar;
		case EConfigVariableType::Vector2:
			return "[ " + std::to_string(CurrentVariable.AsVec2[0]) + " , " + std::to_string(CurrentVariable.AsVec2[1]) + " ]";
		case EConfigVariableType::Vector3:
			return "[ " + std::to_string(CurrentVariable.AsVec3[0]) + " , " +
				std::to_string(CurrentVariable.AsVec3[1]) + " , " + std::to_string(CurrentVariable.AsVec3[2]) + " ]";
	}
	// The code should never reach this path.
	assert(false);
	return std::string();
}
