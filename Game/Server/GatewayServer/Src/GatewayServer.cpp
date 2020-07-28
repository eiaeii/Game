#include "GatewayServer.h"
#include "Log.h"

CGatewayServer::~CGatewayServer()
{
}

bool CGatewayServer::InitServer()
{
	if (!CreateLogSystem())
	{
		printf("[Error]日志服务 初始化失败, Function:%s, Line:%d\n", __FUNCTION__, __LINE__);
		return false;
	}

	m_pZmqContext = zmq_ctx_new();
	if (nullptr == m_pZmqContext)
	{
		SaveAssertLog("zmq_ctx_new 失败");
		return false;
	}

	m_pZmqSocket = zmq_socket(m_pZmqContext, ZMQ_ROUTER);
	if (nullptr == m_pZmqSocket)
	{
		SaveAssertLog("zmq_socket 失败");
		return false;
	}

	int rc = -1;

	rc = zmq_socket_monitor(m_pZmqSocket, "inproc://monitor.rep", ZMQ_EVENT_ALL);
	if (rc < 0)
	{
		SaveAssertLog("error in zmq_socket_monitor, return:%d, error msg:%s", rc, zmq_strerror(errno));
		return false;
	}

	std::thread monitorThread(&CGatewayServer::MonitorFunction, this);
	monitorThread.detach();

	zmq_msg_init(&m_zmqMsg);
	rc = zmq_bind(m_pZmqSocket, "tcp://*:5555");
	if (rc < 0)
	{
		SaveAssertLog("error in zmq_bind, return:%d, error msg:%s", rc, zmq_strerror(errno));
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
	int rc = 0;

	// 消息地址帧
	zmq_msg_t msgAddress;
	rc = zmq_msg_init(&msgAddress);

	int size = zmq_msg_recv(&msgAddress, m_pZmqSocket, 0);
	printf("-------------recv_begin-------------\n");
	uint32_t nMsgFd = zmq_msg_get(&msgAddress, ZMQ_SRCFD);
	printf("msg_from:%d\n", nMsgFd);

	// 消息内容
	zmq_msg_t message;
	rc = zmq_msg_init(&message);
	size = zmq_msg_recv(&message, m_pZmqSocket, 0);
	char szMsg[1024] = { 0 };
	memcpy(szMsg, zmq_msg_data(&message), size);
	printf("msg:%s\n", szMsg);

	// 将消息返回
	zmq_msg_send(&msgAddress, m_pZmqSocket, ZMQ_SNDMORE);
	zmq_msg_send(&message, m_pZmqSocket, 0);

	zmq_msg_close(&msgAddress);
	zmq_msg_close(&message);

	printf("-------------recv_end-------------\n");
}

bool CGatewayServer::BeginStop()
{
	zmq_close(&m_pZmqSocket);
	zmq_term(&m_pZmqContext);
	zmq_msg_close(&m_zmqMsg);

	return true;
}

void CGatewayServer::MonitorFunction()
{
	static char addr[1025];
	int rc;
	printf("starting monitor...\n");
	void *s = zmq_socket(m_pZmqContext, ZMQ_PAIR);
	rc = zmq_connect(s, "inproc://monitor.rep");

	while (true)
	{
		zmq_msg_t message;
		rc = zmq_msg_init(&message);

		int size = zmq_msg_recv(&message, s, 0);
		printf("-------------event_begin-------------\n");
		uint8_t *pMsg = (uint8_t*)zmq_msg_data(&message);
		zmq_event_t event_t;
		memcpy(&event_t.eventID, pMsg, sizeof(event_t.eventID));
		memcpy(&event_t.eventValue, pMsg + sizeof(event_t.eventID), sizeof(event_t.eventValue));
		printf("event_id:%x\n", event_t.eventID);
		printf("event_value:%d\n", event_t.eventValue);


		/*zmq_event_t *pEvent = (zmq_event_t *)zmq_msg_data(&message);
		printf("event_id:%x\n", pEvent->eventID);
		printf("event_value:%d\n", pEvent->eventValue);*/

		char szAddress[128] = { 0 };
		size = zmq_msg_recv(&message, s, 0);
		memcpy(szAddress, zmq_msg_data(&message), size);
		printf("event_address:%s\n", szAddress);

		printf("-------------event_end-------------\n");
		rc = zmq_msg_close(&message);
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	zmq_close(s);
}