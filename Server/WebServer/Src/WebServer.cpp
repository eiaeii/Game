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
		printf("[Error]日志服务 初始化失败! Function:%s, Line:%d\n", __FUNCTION__, __LINE__);
		return false;
	}

	/*if (!CreateLuaEngine())
	{
		SaveAssertLog("Lua引擎 初始化失败！ Function:%s, Line:%d", __FUNCTION__, __LINE__);
		return false;
	}*/

	return true;
}

bool CWebServer::Start()
{
	if (!GetLuaEngine()->LoadLuaFile("./Data/Scripts/WebServer/Main.lua"))
	{
		SaveAssertLog("Lua引擎 启动失败！ Function:%s, Line:%d", __FUNCTION__, __LINE__);
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
