#ifndef LuaDataReader_h__
#define LuaDataReader_h__

#include "LuaState.h"
#include <string>
#include <glm/glm.hpp>

class FLuaDataReader
{
public:
	FLuaDataReader(FLuaState& LuaState);

	FLuaDataReader& ReadGlobalString(std::string& OutResult, const char* Name);
	FLuaDataReader& ReadGlobalBoolean(bool& bOutResult, const char* Name);
	FLuaDataReader& ReadGlobalNumber(float& OutResult, const char* Name);

	FLuaDataReader& ReadString(std::string& OutResult);
	FLuaDataReader& ReadNumber(float& OutResult);
	FLuaDataReader& ReadBoolean(bool& bOutResult);
	FLuaDataReader& ReadVector(glm::vec3& OutResult);

	FLuaDataReader& Pop(int Count = 1);
	FLuaDataReader& CopyValue(int Index = -1);
	FLuaDataReader& GetTable(const char* Name);

	// Pushes value at -1 and key at -2
	bool BeginGlobalTable(const char* Name);
	bool BeginTable();
	bool NextTableElement();
	void EndTable();

	FLuaState& GetState() { return LuaState; }
private:
	FLuaState& LuaState;
};

#endif // LuaDataReader_h__
