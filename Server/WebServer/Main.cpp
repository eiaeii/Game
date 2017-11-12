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
		printf("[Error]��־���� ��ʼ��ʧ��! Function:%s, Line:%d\n", __FUNCTION__, __LINE__);
		return false;
	}

	if (!CLuaEngine::Instance()->Init())
	{
		SaveAssertLog("Lua���� ��ʼ��ʧ�ܣ� Function:%s, Line:%d", __FUNCTION__, __LINE__);
		return false;
	}

	if (!CLuaEngine::Instance()->LoadLuaFile("Scripts/WebServer/Main.lua"))
	{
		SaveAssertLog("Lua���� ����ʧ�ܣ� Function:%s, Line:%d", __FUNCTION__, __LINE__);
		return false;
	}

	if (!CWebServer::Instance()->InitServer(argc, argv))
	{
		SaveAssertLog("WebServer ��ʼ��ʧ�ܣ� Function:%s, Line:%d", __FUNCTION__, __LINE__);
		return false;
	}

	if (!CWebServer::Instance()->Start())
	{
		SaveAssertLog("WebServer ����ʧ�ܣ� Function:%s, Line:%d", __FUNCTION__, __LINE__);
		return false;
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