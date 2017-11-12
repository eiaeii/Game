#include <cstdio>
#include "WebServer.h"
#include <iostream>
#include <string>
#include "Log.h"
#include "LuaEngine.h"

bool StarServer(int argc, char *argv[])
{
	if (!CLog::Instance()->Init())
	{
		printf("[Error]日志服务 初始化失败! Function:%s, Line:%d\n", __FUNCTION__, __LINE__);
		return false;
	}

	if (!CLuaEngine::Instance()->Init())
	{
		SaveAssertLog("Lua引擎 初始化失败！ Function:%s, Line:%d", __FUNCTION__, __LINE__);
		return false;
	}

	if (!CLuaEngine::Instance()->LoadLuaFile("Scripts/WebServer/Main.lua"))
	{
		SaveAssertLog("Lua引擎 启动失败！ Function:%s, Line:%d", __FUNCTION__, __LINE__);
		return false;
	}

	if (!CWebServer::Instance()->InitServer(argc, argv))
	{
		SaveAssertLog("WebServer 初始化失败！ Function:%s, Line:%d", __FUNCTION__, __LINE__);
		return false;
	}

	if (!CWebServer::Instance()->Start())
	{
		SaveAssertLog("WebServer 启动失败！ Function:%s, Line:%d", __FUNCTION__, __LINE__);
		return false;
	}

	printf_s("WebServer 启动成功！\n");
	
	std::string strCMD;
	while (true)
	{
		std::cin >> strCMD;
		if (strCMD == "exit")
		{
			CWebServer::Instance()->Stop();
			break;
		}
		else
			CLuaEngine::Instance()->DoString(strCMD.c_str());
	}

	return true;
}

int main(int argc, char *argv[])
{
	if (!StarServer(argc, argv))
		system("pause");
	return 0;
}