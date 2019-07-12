#include <cstdio>
#include <iostream>
#include <string>
#include "GameClient.h"

bool StarClient()
{
	if (!CGameClient::Instance()->InitClient())
	{
		SaveAssertLog("Client ³õÊ¼»¯Ê§°Ü£¡ Function:%s, Line:%d", __FUNCTION__, __LINE__);
		return false;
	}

	if (!CGameClient::Instance()->Start())
	{
		SaveAssertLog("Client Æô¶¯Ê§°Ü£¡ Function:%s, Line:%d", __FUNCTION__, __LINE__);
		return false;
	}

	printf("Client Start Succeed!\n");

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
		else
		{
			CGameClient::Instance()->SendMsg(strCMD.c_str(), strCMD.length());
		}
	}

	return true;
}

int main()
{
	if (!StarClient())
	{
		printf("Client Start Failed!\n");
	}

	return 0;
}