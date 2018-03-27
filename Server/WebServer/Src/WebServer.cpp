#include "WebServer.h"

CWebServer::~CWebServer()
{
}

bool CWebServer::InitServer(int argc, char * argv[])
{
	argc; argv;
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
