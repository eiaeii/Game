#include <cstdio>
#include <iostream>
#include <string>
#include "GameClient.h"

bool StarServer()
{
	if (!CGameClient::Instance()->InitClient())
	{
		SaveAssertLog("GatewayServer ³õÊ¼»¯Ê§°Ü£¡ Function:%s, Line:%d", __FUNCTION__, __LINE__);
		return false;
	}

	if (!CGameClient::Instance()->Start())
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
			CGameClient::Instance()->Stop();
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