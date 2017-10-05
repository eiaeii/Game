#include <cstdio>
#include "WebServer.h"
#include <iostream>
#include <string>
#include "Log.h"
#include "LuaEngine.h"

void StarServer(int argc, char *argv[])
{
	if (!CLog::Instance()->Init())
	{
		printf("[Error]WebServer LogServer Failed! Function:%s, Line:%d\n", __FUNCTION__, __LINE__);
		return;
	}

	if (!CWebServer::Instance()->InitServer(argc, argv))
	{
		printf("[Error]WebServer Init Failed! Function:%s, Line:%d\n", __FUNCTION__, __LINE__);
		SaveAssertLog("WebServer 初始化失败！ Function:%s, Line:%d", __FUNCTION__, __LINE__);
		return;
	}

	if (!CWebServer::Instance()->Start())
	{
		printf("[Error]WebServer Start Failed! Function:%s, Line:%d\n", __FUNCTION__, __LINE__);
		SaveAssertLog("WebServer 初始化失败！ Function:%s, Line:%d", __FUNCTION__, __LINE__);
		return;
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
	}
}

int main(int argc, char *argv[])
{
	if (!CLog::Instance()->Init())
	{
		printf("[Error]WebServer LogServer Failed! Function:%s, Line:%d\n", __FUNCTION__, __LINE__);
		return 1;
	}

	CLuaEngine::Instance()->Create();

	CLuaEngine::Instance()->SetLuaPath("Scripts/WebServer");
	CLuaEngine::Instance()->LoadLuaFile("Scripts/WebServer/Main.lua");
	CLuaEngine::Instance()->RunLuaFunction("MyTest", 1, "12", std::string("123"));
	//StarServer(argc, argv);
	argc;
	argv;
	system("pause");
	return 0;
}