#ifndef LuaState_h__
#define LuaState_h__
#include "lua.hpp"
#include <assert.h>

class FLuaState
{
public:
	FLuaState();
	~FLuaState();

	void PushNil()							{ lua_pushnil(CurrentState); }
	void PushBoolean(bool bValue)			{ lua_pushboolean(CurrentState, bValue); }
	void PushNumber(double Value)			{ lua_pushnumber(CurrentState, Value); }
	void PushString(const char* Value)		{ lua_pushstring(CurrentState, Value); }
	bool CheckStack(int Size)				{ return lua_checkstack(CurrentState, Size) != 0; }
	void Pop(int Count = 1)					{ lua_pop(CurrentState, Count); }

	bool IsElementBoolean(int Index)		{ return lua_isboolean(CurrentState, Index) != 0; }
	bool IsElementNumber(int Index)			{ return lua_isnumber(CurrentState, Index) != 0; }
	bool IsElementString(int Index)			{ return lua_isstring(CurrentState, Index) != 0; }
	bool isElementTable(int Index)			{ return lua_istable(CurrentState, Index) != 0; }
	int GetElementStringLength(int Index)	{ return lua_rawlen(CurrentState, Index); }

	bool GetElementBoolean(int Index)		{ return lua_toboolean(CurrentState, Index) != 0; }
	double GetElementNumber(int Index)		{ return lua_tonumber(CurrentState, Index); }
	const char* GetElementString(int Index)	{ return lua_tostring(CurrentState, Index); }

	// Note this function assumes that the table is at the top of the stack.
	bool GetTable(const char* ElementName, int TableIndex = -1);
	bool GetTable(int TableIndex = -2);
	bool NextTableElement(int TableIndex = -1);

	int GetStackCount()						{ return lua_gettop(CurrentState); }
	void GetGlobal(const char* Name)		{ lua_getglobal(CurrentState, Name); }

	bool LoadStringCode(char* CodeBuffer, int CodeSize);
	bool LoadStringCode(const char* CodeBuffer);
	bool LoadFile(const char* Filename);

	lua_State* GetState()					{ return CurrentState; }

	void StackDump();
private:
	void DumpError();

	lua_State* CurrentState;
};

#endif // LuaState_h__
