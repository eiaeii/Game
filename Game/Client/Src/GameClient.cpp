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
		SaveAssertLog("zmq_ctx_new 失败");
		return false;
	}

	m_pZmqSocket = zmq_socket(m_pZmqContext, ZMQ_DEALER);
	if (nullptr == m_pZmqSocket)
	{
		SaveAssertLog("zmq_socket 失败");
		return false;
	}

	zmq_msg_init(&m_zmqMsg);
	int rc = zmq_connect(m_pZmqSocket, "tcp://localhost:5555");
	if (rc < 0)
	{
		SaveAssertLog("error in zmq_connect, return:%d, error msg:%s", rc, zmq_strerror(errno));
		return false;
	}
	
	return true;
}

bool CGameClient::Start()
{
	return IThread::Start();
}

void CGameClient::ProcessLogic()
{
	int rc = zmq_msg_recv(&m_zmqMsg, m_pZmqSocket, 0);
	if (rc < 0)
	{
		SaveAssertLog("error in zmq_msg_recv, return:%d, error msg:%s", rc, zmq_strerror(errno));
		return;
	}

	char szMsg[1024] = { 0 };
	auto nSize = zmq_msg_size(&m_zmqMsg);
	memcpy(szMsg, zmq_msg_data(&m_zmqMsg), nSize);
	printf("msg:%s\n", szMsg);
}

bool CGameClient::BeginStop()
{
	zmq_close(&m_pZmqSocket);
	zmq_term(&m_pZmqContext);
	zmq_msg_close(&m_zmqMsg);

	return true;
}

void CGameClient::SendMsg(const char *pMsg, size_t nLength)
{
	int rc = zmq_send(m_pZmqSocket, pMsg, nLength, 0);
	if (rc < 0)
	{
		SaveAssertLog("error in zmq_send, return:%d, error msg:%s", rc, zmq_strerror(errno));
		return;
	}
}
