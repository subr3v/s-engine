#include "LuaState.h"
#include "Vector.h"
#include "FileArchive.h"

FLuaState::FLuaState()
{
	CurrentState = luaL_newstate();
	luaL_openlibs(CurrentState);
}

FLuaState::~FLuaState()
{
	lua_close(CurrentState);
}

bool FLuaState::LoadStringCode(char* CodeBuffer, int CodeSize)
{
	luaL_loadbuffer(CurrentState, CodeBuffer, CodeSize, "line");
	bool bError = lua_pcall(CurrentState, 0, 0, 0) != 0;
	if (bError)
	{
		DumpError();
	}
	return bError == false;
}

bool FLuaState::LoadStringCode(const char* CodeBuffer)
{
	luaL_loadstring(CurrentState, CodeBuffer);
	bool bError = lua_pcall(CurrentState, 0, 0, 0) != 0;
	if (bError)
	{
		DumpError();
	}
	return bError == false;
}

bool FLuaState::LoadFile(const char* Filename)
{
	FFileArchive FileArchive(Filename, false);
	if (FileArchive.IsSuccessful() == false)
		return false;

	TVector<s8> CodeData;
	CodeData.Resize(FileArchive.GetSize());
	FileArchive.Read(CodeData.Data(), FileArchive.GetSize());
	return LoadStringCode(CodeData.Data(), CodeData.Size());
}

bool FLuaState::GetTable(const char* ElementName, int TableIndex)
{
	if (isElementTable(TableIndex) == false)
		return false;

	PushString(ElementName);
	return lua_gettable(CurrentState, TableIndex - 1) != 0; // This is because we assume that table was at -1 before.
}

bool FLuaState::GetTable(int TableIndex)
{
	return lua_gettable(CurrentState, TableIndex) != 0; // This is because we assume that table was at -1 before.
}

bool FLuaState::NextTableElement(int TableIndex /*= -1*/)
{
	return lua_next(CurrentState, TableIndex) != 0;
}

// Note: quick implementation definitely not portable.
int __cdecl PrintOutput(const char *format, ...)
{
	char str[1024];

	va_list argptr;
	va_start(argptr, format);
	int ret = vsnprintf_s(str, sizeof(str), format, argptr);
	va_end(argptr);

	OutputDebugStringA(str);

	return ret;
}

void FLuaState::StackDump()
{
	int top = lua_gettop(CurrentState);

	PrintOutput("total in stack %d\n", top);

	for (int i = 1; i <= top; i++)
	{ 
		/* repeat for each level */
		int t = lua_type(CurrentState, i);
		switch (t) 
		{
			case LUA_TSTRING: /* strings */
				PrintOutput("string: '%s'\n", lua_tostring(CurrentState, i));
				break;
			case LUA_TBOOLEAN: /* booleans */
				PrintOutput("boolean %s\n", lua_toboolean(CurrentState, i) ? "true" : "false");
				break;
			case LUA_TNUMBER: /* numbers */
				PrintOutput("number: %g\n", lua_tonumber(CurrentState, i));
				break;
			default: /* other values */
				PrintOutput("%s\n", lua_typename(CurrentState, t));
				break;
		}
		PrintOutput(" "); /* put a separator */
	}
	PrintOutput("\n"); /* end the listing */
}

void FLuaState::DumpError()
{
	PrintOutput("%s", lua_tostring(CurrentState, -1));
}
