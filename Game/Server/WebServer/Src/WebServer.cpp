#include "WebServer.h"
#include "Log.h"

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

	return true;
}

bool CWebServer::Start()
{
	return IThread::Start();
}

void CWebServer::ProcessLogic()
{
}

bool CWebServer::BeginStop()
{
	return true;
}
