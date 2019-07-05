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

	m_pZmqContext = zmq_init(4);
	if (nullptr == m_pZmqContext)
	{
		printf("[Error]zmq_init 失败! Function:%s, Line:%d\n", __FUNCTION__, __LINE__);
		return false;
	}

	m_pZmqSocket = zmq_socket(m_pZmqContext, ZMQ_REP);
	if (nullptr == m_pZmqSocket)
	{
		printf("[Error]zmq_socket 失败! Function:%s, Line:%d\n", __FUNCTION__, __LINE__);
		return false;
	}

	zmq_bind(m_pZmqSocket, "tcp://*5555");

	return true;
}

bool CGatewayServer::Start()
{
	return IThread::Start();
}

void CGatewayServer::ProcessLogic()
{
	zmq_msg_t msg;
	zmq_msg_init(&msg);
	zmq_recvmsg(m_pZmqSocket, &msg, 0);
	printf("ProcessLogic recvmsg");
	zmq_msg_close(&msg);
}

bool CGatewayServer::BeginStop()
{
	zmq_close(&m_pZmqSocket);
	zmq_term(&m_pZmqContext);

	return true;
}
