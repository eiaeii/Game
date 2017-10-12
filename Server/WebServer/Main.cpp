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
		printf("[Error]��־�����ʼ��ʧ��! Function:%s, Line:%d\n", __FUNCTION__, __LINE__);
		return;
	}

	if (!CWebServer::Instance()->InitServer(argc, argv))
	{
		SaveAssertLog("WebServer ��ʼ��ʧ�ܣ� Function:%s, Line:%d", __FUNCTION__, __LINE__);
		return;
	}

	if (!CWebServer::Instance()->Start())
	{
		SaveAssertLog("WebServer ����ʧ�ܣ� Function:%s, Line:%d", __FUNCTION__, __LINE__);
		return;
	}

	if (!CLuaEngine::Instance()->Init())
	{
		SaveAssertLog("Lua���� ��ʼ��ʧ�ܣ� Function:%s, Line:%d", __FUNCTION__, __LINE__);
		return;
	}

	if (!CLuaEngine::Instance()->LoadLuaFile("Scripts/WebServer/Main.lua"))
	{
		SaveAssertLog("Lua���� ����ʧ�ܣ� Function:%s, Line:%d", __FUNCTION__, __LINE__);
		return;
	}

	printf_s("WebServer �����ɹ���\n");

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
	StarServer(argc, argv);
	return 0;
}