/***********************************************************************
** 作者：何水大(765865368@qq.com)
** 时间：2017年10月16日
** 描述：Lua引擎
************************************************************************/

#ifndef _LUAENGINE_H

#include <array>
#include <iostream>
#include <float.h>

#include "Log.h"
#ifdef __cplusplus
extern "C"
{
#include "../LuaJIT/lauxlib.h"
#include "../LuaJIT/lua.h"
#include "../LuaJIT/luajit.h"
#include "../LuaJIT/lualib.h"
}
#endif // __cplusplus

#define _LUAENGINE_H

struct LuaParam
{
	LuaParam()
		:m_btType(LUA_TNIL)
	{
	}

	explicit LuaParam(double value)
		:m_btType(LUA_TNUMBER), m_value(value)
	{
	}

	explicit LuaParam(const char *value)
		:m_btType(LUA_TSTRING), m_value(value)
	{
	}

	~LuaParam() {}

	operator char()
	{
		return (char)m_value.num;
	}

	operator unsigned char()
	{
		return (unsigned char)m_value.num;
	}

	operator short()
	{
		return (short)m_value.num;
	}

	operator unsigned short()
	{
		return (unsigned short)m_value.num;
	}

	operator int()
	{
		return (int)m_value.num;
	}

	operator unsigned int()
	{
		return (unsigned int)m_value.num;
	}

	operator float()
	{
		return (float)m_value.num;
	}

	operator double()
	{
		return m_value.num;
	}

	operator bool()
	{
		return m_value.num > -FLT_EPSILON && m_value.num < FLT_EPSILON;
	}

	operator const char*()
	{
		return m_value.str.c_str();
	}

	operator std::string()
	{
		return m_value.str;
	}

	unsigned char GetValueType()
	{
		return m_btType;
	}

	void operator = (double value)
	{
		m_btType = LUA_TNUMBER;
		m_value.num = value;
	}

	void operator = (const char *value)
	{
		m_btType = LUA_TSTRING;
		m_value.str = value;
	}

	friend std::ostream & operator << (std::ostream &out, const LuaParam &value)
	{
		switch (value.m_btType)
		{
		case LUA_TNUMBER:
			out << value.m_value.num;
			break;
		case LUA_TSTRING:
			out << value.m_value.str;
			break;
		default:
			out << "未初始化！";
			break;
		}

		return out;
	}

private:
	unsigned char m_btType;

	union Value
	{
		Value()
		{

		}

		Value(double value) :num(value)
		{

		}

		Value(const char *value) :str(value)
		{

		}

		~Value() {}

		double num;
		std::string str;

	}m_value;
};

class CLuaEngine
{
public:

	bool Init();

	bool LoadLuaFile(const char *szLuaFileName);

	template<typename... T>
	bool RunLuaFunction(const char *szFunctionName, const T&... args)
	{
		int nTop = lua_gettop(m_pLuaState);
		std::string strError = "【Lua】";

		try
		{
			lua_getglobal(m_pLuaState, szFunctionName);
			if (!lua_isfunction(m_pLuaState, -1))
			{
				strError = strError + "没有找到函数：" + szFunctionName;
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

			auto temp = {0, (funcPushValue(args), 0)...};

			int nErrorCode = lua_pcall(m_pLuaState, nInNum, 0, 0);
			if (0 == nErrorCode)
			{
				lua_settop(m_pLuaState, nTop);
				return true;
			}

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

	template<size_t N, typename... T>
	bool RunLuaFunctionReturnArray(const char *szFunctionName, std::array<LuaParam, N> &arrResult, const T&... args)
	{
		int nTop = lua_gettop(m_pLuaState);
		std::string strError = "【Lua】";

		try
		{
			lua_getglobal(m_pLuaState, szFunctionName);
			if (!lua_isfunction(m_pLuaState, -1))
			{
				strError = strError + "没有找到函数：" + szFunctionName;
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

			auto temp = { 0, (funcPushValue(args), 0)... };

			int nErrorCode = lua_pcall(m_pLuaState, nInNum, N, 0);
			if (0 == nErrorCode)
			{
				int nCount = N;
				if (nCount <= 0)
				{
					lua_settop(m_pLuaState, nTop);
					return true;
				}

				for (int i = 0; i < nCount; ++i)
				{
					auto nType = lua_type(m_pLuaState, -nCount+i);
					switch (nType)
					{
					case LUA_TNUMBER:
						arrResult[i] = lua_tonumber(m_pLuaState, -nCount + i);
						break;
					case LUA_TBOOLEAN:
						arrResult[i] = lua_toboolean(m_pLuaState, -nCount + i);
						break;
					case LUA_TSTRING:
						arrResult[i] = lua_tostring(m_pLuaState, -nCount + i);
						break;
					case LUA_TNIL:
						lua_touserdata(m_pLuaState, -nCount + i);
						arrResult[i] = 0.0f;
						break;
					default:
						char szError[64] = { 0 };
						sprintf(szError, "【Lua】未支持的返回值类型，类型ID：%d", nType);
						SaveAssertLog(szError);
						lua_touserdata(m_pLuaState, -nCount + i);
						arrResult[i] = 0.0f;
						break;
					}
				}

				lua_settop(m_pLuaState, nTop);
				return true;
			}
			
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

	void DoString(const char* pszCode);

private:
	
	void PushValue(const bool &value)
	{
		lua_pushboolean(m_pLuaState, value);
	}

	void PushValue(const char &value)
	{
		lua_pushinteger(m_pLuaState, value);
	}

	void PushValue(const unsigned char &value)
	{
		lua_pushinteger(m_pLuaState, value);
	}

	void PushValue(const short &value)
	{
		lua_pushinteger(m_pLuaState, value);
	}

	void PushValue(const unsigned short&value)
	{
		lua_pushinteger(m_pLuaState, value);
	}

	void PushValue(const int &value)
	{
		lua_pushinteger(m_pLuaState, value);
	}

	void PushValue(const unsigned int &value)
	{
		lua_pushinteger(m_pLuaState, value);
	}

	void PushValue(const long &value)
	{
		lua_pushinteger(m_pLuaState, value);
	}

	void PushValue(const unsigned long &value)
	{
		lua_pushinteger(m_pLuaState, value);
	}

	void PushValue(const long long &value)
	{
		lua_pushinteger(m_pLuaState, value);
	}

	void PushValue(const unsigned long long &value)
	{
		lua_pushinteger(m_pLuaState, value);
	}

	void PushValue(const float &value)
	{
		lua_pushnumber(m_pLuaState, value);
	}

	void PushValue(const double &value)
	{
		lua_pushnumber(m_pLuaState, value);
	}

	void PushValue(const char *szValue)
	{
		lua_pushstring(m_pLuaState, szValue);
	}

	void PushValue(const std::string &strValue)
	{
		lua_pushstring(m_pLuaState, strValue.c_str());
	}

	void AddSearchPath(const char* path);

	void SetLuaSearchPath();

private:
	lua_State *m_pLuaState;
};

extern CLuaEngine *g_pLuaEngine;
SERVER_API CLuaEngine* CreateLuaEngine();
SERVER_API CLuaEngine* GetLuaEngine();

#endif // _LUAENGINE_H

