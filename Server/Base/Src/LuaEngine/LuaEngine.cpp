#include "LuaEngine.h"

bool CLuaEngine::Create()
{
	m_pLuaState = lua_open();
	if (nullptr == m_pLuaState)
	{
		SaveAssertLog("Lua≥ı ºªØ ß∞‹£°");
		return false;
	}
	
	luaopen_base(m_pLuaState);
	luaopen_math(m_pLuaState);
	luaopen_string(m_pLuaState);
	luaopen_table(m_pLuaState);
	luaopen_io(m_pLuaState);
	luaopen_os(m_pLuaState);
	luaopen_package(m_pLuaState);
	luaopen_bit(m_pLuaState);
	luaopen_jit(m_pLuaState);
	luaopen_ffi(m_pLuaState);
	luaL_openlibs(m_pLuaState);
	
	return true;
}

bool CLuaEngine::LoadLuaFile(const char* szLuaFileName)
{
	if (szLuaFileName == NULL)
	{
		return false;
	}

	int top = lua_gettop(m_pLuaState);
	try
	{

		//int nResult = dofile(m_pLuaState, szLuaFileName);
		int nResult = luaL_loadfile(m_pLuaState, szLuaFileName);
		if (nResult == 0)
		{
			nResult = lua_pcall(m_pLuaState, 0, 0, 0);
			if (nResult == 0)
			{
				lua_settop(m_pLuaState, top);

				return true;
			}
		}
	}
	catch (...)
	{

	}

	const char* pszErrInfor = lua_tostring(m_pLuaState, -1);
	SaveAssertLog(pszErrInfor);

	lua_settop(m_pLuaState, top);

	return false;
}