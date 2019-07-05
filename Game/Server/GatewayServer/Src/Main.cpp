#include <cstdio>
#include <iostream>
#include <string>
#include "GatewayServer.h"

bool StarServer()
{
	if (!CGatewayServer::Instance()->InitServer())
	{
		SaveAssertLog("GatewayServer ³õÊ¼»¯Ê§°Ü£¡ Function:%s, Line:%d", __FUNCTION__, __LINE__);
		return false;
	}

	if (!CGatewayServer::Instance()->Start())
	{
		SaveAssertLog("GatewayServer Æô¶¯Ê§°Ü£¡ Function:%s, Line:%d", __FUNCTION__, __LINE__);
		return false;
	}

	printf("GatewayServer Start Succeed!\n");

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
			CGatewayServer::Instance()->Stop();
			break;
		}
	}

	return true;
}

int main()
{
	if (!StarServer())
	{
		printf("GatewayServer Start Failed!\n");
	}

	return 0;
}