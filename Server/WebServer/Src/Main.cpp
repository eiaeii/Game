#include <cstdio>
#include "WebServer.h"
#include <iostream>
#include <string>
#include "Log.h"
#include "LuaEngine.h"

bool StarServer()
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

	if (!CLuaEngine::Instance()->LoadLuaFile("./Data/Scripts/WebServer/Main.lua"))
	{
		SaveAssertLog("Lua���� ����ʧ�ܣ� Function:%s, Line:%d", __FUNCTION__, __LINE__);
		return false;
	}
	
	if (!CWebServer::Instance()->InitServer())
	{
		SaveAssertLog("WebServer ��ʼ��ʧ�ܣ� Function:%s, Line:%d", __FUNCTION__, __LINE__);
		return false;
	}

	if (!CWebServer::Instance()->Start())
	{
		SaveAssertLog("WebServer ����ʧ�ܣ� Function:%s, Line:%d", __FUNCTION__, __LINE__);
		return false;
	}

	printf("WebServer Start Succeed!\n");
	
	std::string strCMD;
	while (true)
	{
		std::getline(std::cin, strCMD);
		if (strCMD.empty())
		{
			continue;
		}
		
		if (strCMD == "exit")
		{
			CWebServer::Instance()->Stop();
			break;
		}
		else
		{
			CLuaEngine::Instance()->DoString(strCMD.c_str());
		}
	}

	return true;
}

int main()
{
	if (!StarServer())
	{
		printf("WebServer Start Failed!\n");
	}

	return 0;
}