/***********************************************************************
** 作者：何水大(765865368@qq.com)
** 时间：2017年10月16日
** 描述：Lua引擎
************************************************************************/

#include "LuaEngine.h"
extern CLuaEngine *g_pLuaEngine = nullptr;

bool CLuaEngine::Init()
{
	m_pLuaState = lua_open();
	if (nullptr == m_pLuaState)
	{
		SaveAssertLog("Lua初始化失败！");
		return false;
	}
	
	luaL_openlibs(m_pLuaState);

	SetLuaSearchPath();

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

	std::string strError = "【Lua】";
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

void CLuaEngine::SetLuaSearchPath()
{
	auto curPath = std::experimental::filesystem::current_path().string();
	
	for (auto &p : std::experimental::filesystem::recursive_directory_iterator(curPath))
	{
		if (std::experimental::filesystem::is_directory(p))
			AddSearchPath(p.path().string().c_str());
	}
}

void CLuaEngine::DoString(const char* pszCode)
{
	luaL_dostring(m_pLuaState, pszCode);
}

CLuaEngine* CreateLuaEngine()
{
	if (nullptr != g_pLuaEngine)
	{
		return g_pLuaEngine;
	}

	g_pLuaEngine = new CLuaEngine();
	if (nullptr == g_pLuaEngine)
	{
		SaveAssertLog("Lua创建失败！");
		return nullptr;
	}

	g_pLuaEngine->Init();

	return g_pLuaEngine;
}

CLuaEngine* GetLuaEngine()
{
	if (nullptr == g_pLuaEngine)
	{
		return CreateLuaEngine();
	}

	return g_pLuaEngine;
}