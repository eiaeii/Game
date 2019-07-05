#include "GatewayServer.h"
#include "Log.h"

GatewayServer::~GatewayServer()
{
}

bool GatewayServer::InitServer()
{
	if (!CreateLogSystem())
	{
		printf("[Error]��־���� ��ʼ��ʧ��! Function:%s, Line:%d\n", __FUNCTION__, __LINE__);
		return false;
	}

	return true;
}

bool GatewayServer::Start()
{
	return IThread::Start();
}

void GatewayServer::ProcessLogic()
{
}

bool GatewayServer::BeginStop()
{
	return true;
}
