#include "LuaEngine.h"
#include <typeinfo>

bool CLuaEngine::Create()
{
	m_pLuaState = lua_open();
	if (nullptr == m_pLuaState)
	{
		SaveAssertLog("Lua³õÊ¼»¯Ê§°Ü£¡");
		return false;
	}
	
	luaopen_base(m_pLuaState);
	luaopen_math(m_pLuaState);
	luaopen_string(m_pLuaState);
	luaopen_table(m_pLuaState);
	luaopen_io(m_pLuaState);
	luaopen_os(m_pLuaState);
	//luaopen_package(m_pLuaState);
	//luaopen_debug(m_pLuaState);
	//luaopen_bit(m_pLuaState);
	luaopen_jit(m_pLuaState);
	luaopen_ffi(m_pLuaState);
	luaL_openlibs(m_pLuaState);

	return true;
}

bool CLuaEngine::LoadLuaFile(const char *szLuaFileName)
{
	if (szLuaFileName == NULL)
	{
		return false;
	}

	int nTop = lua_gettop(m_pLuaState);
	try
	{
		int nErrorCode = luaL_loadfile(m_pLuaState, szLuaFileName);
		if (0 == nErrorCode)
		{
			nErrorCode = lua_pcall(m_pLuaState, 0, 0, 0);
			if (0 == nErrorCode)
			{
				lua_settop(m_pLuaState, nTop);

				return true;
			}
		}
	}
	catch (...)
	{

	}

	std::string strError = "¡¾Lua¡¿";
	const char* pszErrInfor = lua_tostring(m_pLuaState, -1);
	strError += pszErrInfor;
	SaveAssertLog(strError.c_str());

	lua_settop(m_pLuaState, nTop);

	return false;
}

void CLuaEngine::AddSearchPath(const char* path)
{
	lua_getglobal(m_pLuaState, "package");                                  /* L: package */
	lua_getfield(m_pLuaState, -1, "path");                /* get package.path, L: package path */
	const char* cur_path = lua_tostring(m_pLuaState, -1);
	lua_pushfstring(m_pLuaState, "%s;%s/?.lua", cur_path, path);            /* L: package path newpath */
	lua_setfield(m_pLuaState, -3, "path");          /* package.path = newpath, L: package path */
	lua_pop(m_pLuaState, 2);                                                /* L: - */
}

void CLuaEngine::SetLuaPath(const char* path)
{
	auto curPath = std::experimental::filesystem::current_path().string();
	curPath += "/";
	curPath += path;

	AddSearchPath(curPath.c_str());

	for (auto p : std::experimental::filesystem::recursive_directory_iterator(curPath))
	{
		if (std::experimental::filesystem::is_directory(p))
			AddSearchPath(p.path().string().c_str());
	}
}