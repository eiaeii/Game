/***********************************************************************
** 作者：何水大(765865368@qq.com)
** 时间：2017年8月20日
** 描述：Lua引擎
************************************************************************/

#include "Singleton.hpp"
#include "Log.h"

#ifdef __cplusplus
#include "lauxlib.h"
#include "lua.h"
#include "luajit.h"
#include "lualib.h"
#endif // __cplusplus

#ifndef _LUAENGINE_H
#define _LUAENGINE_H

class CLuaEngine : public CSingleton<CLuaEngine>
{
public:
	bool Create();
	bool LoadLuaFile(const char* szLuaFileName);
private:
	lua_State *m_pLuaState;
};

#endif // _LUAENGINE_H

