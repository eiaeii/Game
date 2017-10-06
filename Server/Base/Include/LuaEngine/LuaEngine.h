/***********************************************************************
** ���ߣ���ˮ��(765865368@qq.com)
** ʱ�䣺2017��8��20��
** ������Lua����
************************************************************************/

#include "Singleton.hpp"
#include "Log.h"

#ifdef __cplusplus
extern "C"
{
#include "lauxlib.h"
#include "lua.h"
#include "luajit.h"
#include "lualib.h"
}
#endif // __cplusplus

#ifndef _LUAENGINE_H
#define _LUAENGINE_H

class CLuaEngine : public CSingleton<CLuaEngine>
{
public:

	bool Create();

	bool LoadLuaFile(const char *szLuaFileName);

	template<typename... T>
	inline bool CLuaEngine::RunLuaFunction(const char *szFunctionName, const T&... args)
	{
		int nTop = lua_gettop(m_pLuaState);
		std::string strError = "��Lua��";

		try
		{
			lua_getglobal(m_pLuaState, szFunctionName);
			if (!lua_isfunction(m_pLuaState, -1))
			{
				strError = strError + "û���ҵ�������" + szFunctionName;
				SaveAssertLog(strError.c_str());

				lua_settop(m_pLuaState, nTop);
				return false;
			}

			int nInNum = 0;

			auto funcPushValue = [&](auto & value)
			{
				++nInNum;
				PushValue(value);
			};

			auto temp = { 0, (funcPushValue(args), 0)... , 0 };

			lua_pcall(m_pLuaState, nInNum, 0, 0);

			lua_settop(m_pLuaState, nTop);

			return true;
		}
		catch (...)
		{

		}

		const char* pszErrInfor = lua_tostring(m_pLuaState, -1);
		strError += pszErrInfor;
		SaveAssertLog(strError.c_str());

		lua_settop(m_pLuaState, nTop);

		return false;
	}

	void SetLuaPath(const char* path);

private:
	
	void inline PushValue(const bool &value)
	{
		lua_pushboolean(m_pLuaState, value);
	}

	void inline PushValue(const char &value)
	{
		lua_pushinteger(m_pLuaState, value);
	}

	void inline PushValue(const unsigned char &value)
	{
		lua_pushinteger(m_pLuaState, value);
	}

	void inline PushValue(const short &value)
	{
		lua_pushinteger(m_pLuaState, value);
	}

	void inline PushValue(const unsigned short&value)
	{
		lua_pushinteger(m_pLuaState, value);
	}

	void inline PushValue(const int &value)
	{
		lua_pushinteger(m_pLuaState, value);
	}

	void inline PushValue(const unsigned int &value)
	{
		lua_pushinteger(m_pLuaState, value);
	}

	void inline PushValue(const long &value)
	{
		lua_pushinteger(m_pLuaState, value);
	}

	void inline PushValue(const unsigned long &value)
	{
		lua_pushinteger(m_pLuaState, value);
	}

	void inline PushValue(const long long &value)
	{
		lua_pushinteger(m_pLuaState, value);
	}

	void inline PushValue(const unsigned long long &value)
	{
		lua_pushinteger(m_pLuaState, value);
	}

	void inline PushValue(const float &value)
	{
		lua_pushnumber(m_pLuaState, value);
	}

	void inline PushValue(const double &value)
	{
		lua_pushnumber(m_pLuaState, value);
	}

	void inline PushValue(const char *szValue)
	{
		lua_pushstring(m_pLuaState, szValue);
	}

	void inline PushValue(const std::string &strValue)
	{
		lua_pushstring(m_pLuaState, strValue.c_str());
	}

	void AddSearchPath(const char* path);

private:
	lua_State *m_pLuaState;
};

#endif // _LUAENGINE_H

