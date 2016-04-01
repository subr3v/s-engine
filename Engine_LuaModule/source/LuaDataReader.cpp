#include "LuaDataReader.h"

FLuaDataReader::FLuaDataReader(FLuaState& LuaState) : LuaState(LuaState)
{

}

FLuaDataReader& FLuaDataReader::ReadGlobalString(std::string& OutResult, const char* Name)
{
	LuaState.GetGlobal(Name);
	return ReadString(OutResult).Pop();
}

FLuaDataReader& FLuaDataReader::ReadGlobalBoolean(bool& bOutResult, const char* Name)
{
	LuaState.GetGlobal(Name);
	return ReadBoolean(bOutResult).Pop();
}

FLuaDataReader& FLuaDataReader::ReadGlobalNumber(float& OutResult, const char* Name)
{
	LuaState.GetGlobal(Name);
	return ReadNumber(OutResult).Pop();
}

FLuaDataReader& FLuaDataReader::ReadString(std::string& OutResult)
{
	OutResult = LuaState.GetElementString(-1);
	return *this;
}

FLuaDataReader& FLuaDataReader::ReadNumber(float& OutResult)
{
	OutResult = (float)LuaState.GetElementNumber(-1);
	return *this;
}

FLuaDataReader& FLuaDataReader::ReadBoolean(bool& bOutResult)
{
	bOutResult = LuaState.GetElementBoolean(-1);
	return *this;
}

bool FLuaDataReader::BeginGlobalTable(const char* Name)
{
	LuaState.GetGlobal(Name);
	return BeginTable();
}

bool FLuaDataReader::BeginTable()
{
	LuaState.PushNil();
	if (LuaState.NextTableElement(-2))
	{
		return true;
	}
	else
	{
		LuaState.Pop(2);
		return false;
	}
}

bool FLuaDataReader::NextTableElement()
{
	LuaState.Pop();
	bool bNextTableValid = LuaState.NextTableElement(-2);
	return bNextTableValid;
}

void FLuaDataReader::EndTable()
{
	LuaState.Pop(1);
}

FLuaDataReader& FLuaDataReader::GetTable(const char* Name)
{
	bool bResult = LuaState.GetTable(Name, -1);
	assert(bResult);
	return *this;
}

FLuaDataReader& FLuaDataReader::ReadVector(glm::vec3& OutResult)
{
	GetTable("x").ReadNumber(OutResult.x).Pop();
	GetTable("y").ReadNumber(OutResult.y).Pop();
	GetTable("z").ReadNumber(OutResult.z).Pop();
	return *this;
}

FLuaDataReader& FLuaDataReader::Pop(int Count)
{
	LuaState.Pop(Count);
	return *this;
}

FLuaDataReader& FLuaDataReader::CopyValue(int Index)
{
	lua_pushvalue(LuaState.GetState(), Index);
	return *this;
}

