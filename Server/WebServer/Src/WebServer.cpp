#include "WebServer.h"
#include "Log.h"
#include "LuaEngine.h"

CWebServer::~CWebServer()
{
}

bool CWebServer::InitServer()
{
	if (!CreateLogSystem())
	{
		printf("[Error]��־���� ��ʼ��ʧ��! Function:%s, Line:%d\n", __FUNCTION__, __LINE__);
		return false;
	}

	/*if (!CreateLuaEngine())
	{
		SaveAssertLog("Lua���� ��ʼ��ʧ�ܣ� Function:%s, Line:%d", __FUNCTION__, __LINE__);
		return false;
	}*/

	return true;
}

bool CWebServer::Start()
{
	if (!GetLuaEngine()->LoadLuaFile("./Data/Scripts/WebServer/Main.lua"))
	{
		SaveAssertLog("Lua���� ����ʧ�ܣ� Function:%s, Line:%d", __FUNCTION__, __LINE__);
		return false;
	}

	return IThread::Start();
}

void CWebServer::ProcessLogic()
{
}

bool CWebServer::BeginStop()
{
	return true;
}
