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

	m_pZmqContext = zmq_ctx_new();
	if (nullptr == m_pZmqContext)
	{
		printf("[Error]zmq_ctx_new 失败! Function:%s, Line:%d\n", __FUNCTION__, __LINE__);
		return false;
	}

	m_pZmqSocket = zmq_socket(m_pZmqContext, ZMQ_ROUTER);
	if (nullptr == m_pZmqSocket)
	{
		printf("[Error]zmq_socket 失败! Function:%s, Line:%d\n", __FUNCTION__, __LINE__);
		return false;
	}

	zmq_msg_init(&m_zmqMsg);
	int rc = zmq_bind(m_pZmqSocket, "tcp://*:5555");
	if (rc < 0)
	{
		printf("error in zmq_bind: %s\n", zmq_strerror(errno));
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
	/*int rc = zmq_msg_recv(&m_zmqMsg, m_pZmqSocket, 0);
	if (rc < 0)
	{
		printf("error in zmq_msg_recv: %s\n", zmq_strerror(errno));
		return;
	}

	char szMsg[128] = { 0 };
	memcpy(szMsg, zmq_msg_data(&m_zmqMsg), zmq_msg_size(&m_zmqMsg));
	printf(szMsg);
	printf("\n");*/
	s_dump();
}

bool CGatewayServer::BeginStop()
{
	zmq_close(&m_pZmqSocket);
	zmq_term(&m_pZmqContext);
	zmq_msg_close(&m_zmqMsg);

	return true;
}
