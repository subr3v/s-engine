#ifndef ConfigVariable_h__
#define ConfigVariable_h__

#include <string>
#include <glm/glm.hpp>
#include "CoreTypes.h"

enum class EConfigVariableType
{
	Boolean,
	Integer,
	Float,
	String,
	Vector2,
	Vector3,
};

class FConfigVariable
{
public:
	FConfigVariable(ConstantString Category, ConstantString Name, bool DefaultValue);
	FConfigVariable(ConstantString Category, ConstantString Name, int32 DefaultValue);
	FConfigVariable(ConstantString Category, ConstantString Name, float DefaultValue);
	FConfigVariable(ConstantString Category, ConstantString Name, const std::string& DefaultValue);
	FConfigVariable(ConstantString Category, ConstantString Name, const char* DefaultValue);
	FConfigVariable(ConstantString Category, ConstantString Name, glm::vec2 DefaultValue);
	FConfigVariable(ConstantString Category, ConstantString Name, glm::vec3 DefaultValue);

	void FromString(const std::string& Text);
	std::string ToString() const;

	EConfigVariableType GetType() const;
	ConstantString GetName() const;
	ConstantString GetCategory() const;

	int32& AsInt();
	float& AsFloat();
	std::string& AsString();
	glm::vec2& AsVec2();
	glm::vec3& AsVec3();
	bool& AsBoolean();

	static const int kStringMaxLength = 256;
private:
	FConfigVariable(FConfigVariable&& Other);
	FConfigVariable(const FConfigVariable& Other);
	FConfigVariable& operator=(const FConfigVariable& Other);

	union FVariableHolder
	{
		bool AsBoolean;
		int32 AsInt;
		float AsFloat;
		float AsVec2[2];
		float AsVec3[3];

		~FVariableHolder() { }
	};

	std::string StringVar;
	FVariableHolder CurrentVariable;
	EConfigVariableType VariableType;
	ConstantString Name;
	ConstantString Category;
};

#endif // ConfigVariable_h__
