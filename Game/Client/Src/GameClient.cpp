#include "GameClient.h"
#include "Log.h"

CGameClient::~CGameClient()
{
}

bool CGameClient::InitClient()
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

	m_pZmqSocket = zmq_socket(m_pZmqContext, ZMQ_REP);
	if (nullptr == m_pZmqSocket)
	{
		printf("[Error]zmq_socket 失败! Function:%s, Line:%d\n", __FUNCTION__, __LINE__);
		return false;
	}

	zmq_connect(m_pZmqSocket, "tcp://localhost:5555");
	
	return true;
}

bool CGameClient::Start()
{
	return IThread::Start();
}

void CGameClient::ProcessLogic()
{
	zmq_msg_t msg;
	zmq_msg_init(&msg);
	zmq_recvmsg(m_pZmqSocket, &msg, 0);
	printf("ProcessLogic recvmsg");
	zmq_msg_close(&msg);
}

bool CGameClient::BeginStop()
{
	zmq_close(&m_pZmqSocket);
	zmq_term(&m_pZmqContext);

	return true;
}

void CGameClient::SendMsg(const char *pMsg, size_t nLength)
{
	zmq_send(m_pZmqSocket, pMsg, nLength, ZMQ_DONTWAIT);
}
