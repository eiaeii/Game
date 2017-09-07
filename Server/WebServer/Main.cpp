#include <cstdio>
#include "WebServer.h"
#include <iostream>
#include <string>
#include "Log.h"

void StarServer(int argc, char *argv[])
{
	if (!CLog::Instance()->Init("WebServer"))
	{
		printf("[Error]WebServer LogServer Failed! Function:%s, Line:%d\n", __FUNCTION__, __LINE__);
		return;
	}

	if (!CWebServer::Instance()->InitServer(argc, argv))
	{
		printf("[Error]WebServer Init Failed! Function:%s, Line:%d\n", __FUNCTION__, __LINE__);
		SaveAssertLog("WebServer ��ʼ��ʧ�ܣ� Function:%s, Line:%d", __FUNCTION__, __LINE__);
		return;
	}

	if (!CWebServer::Instance()->Start())
	{
		printf("[Error]WebServer Start Failed! Function:%s, Line:%d\n", __FUNCTION__, __LINE__);
		SaveAssertLog("WebServer ��ʼ��ʧ�ܣ� Function:%s, Line:%d", __FUNCTION__, __LINE__);
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