/***********************************************************************
** ���ߣ���ˮ��(765865368@qq.com)
** ʱ�䣺2017��8��20��
** ������Lua����
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

