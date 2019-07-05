#include "GatewayServer.h"
#include "Log.h"

CGatewayServer::~CGatewayServer()
{
}

bool CGatewayServer::InitServer()
{
	if (!CreateLogSystem())
	{
		printf("[Error]日志服务 初始化失败! Function:%s, Line:%d\n", __FUNCTION__, __LINE__);
		return false;
	}

	return true;
}

bool CGatewayServer::Start()
{
	return IThread::Start();
}

void CGatewayServer::ProcessLogic()
{
}

bool CGatewayServer::BeginStop()
{
	return true;
}
