#include <cstdio>
#include <iostream>
#include <string>
#include <locale.h>

#include "LuaEngine.h"
#include "WebServer.h"
//#include "zmq.h"

bool StarServer()
{
	if (!CWebServer::Instance()->InitServer())
	{
		SaveAssertLog("WebServer ³õÊ¼»¯Ê§°Ü£¡ Function:%s, Line:%d", __FUNCTION__, __LINE__);
		return false;
	}

	if (!CWebServer::Instance()->Start())
	{
		SaveAssertLog("WebServer Æô¶¯Ê§°Ü£¡ Function:%s, Line:%d", __FUNCTION__, __LINE__);
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
		else if (strCMD.find(".lua") != std::string::npos)
		{
			GetLuaEngine()->LoadLuaFile(strCMD.c_str());
		}
		else
		{
			GetLuaEngine()->DoString(strCMD.c_str());
		}
	}

	return true;
}

int main()
{
	setlocale(LC_ALL, "zh_CN.UTF-8");
	if (!StarServer())
	{
		printf("WebServer Start Failed!\n");
	}

	return 0;
}