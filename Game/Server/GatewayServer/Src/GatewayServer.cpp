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
	/*int rc = zmq_msg_recv(&m_zmqMsg, m_pZmqSocket, 0);
	if (rc < 0)
	{
		printf("error in zmq_msg_recv: %s\n", zmq_strerror(errno));
		return;
	}

	char szMsg[128] = { 0 };
	memcpy(szMsg, zmq_msg_data(&m_zmqMsg), zmq_msg_size(&m_zmqMsg));
	printf(szMsg);*/
	s_dump();
}

bool CGatewayServer::BeginStop()
{
	zmq_close(&m_pZmqSocket);
	zmq_term(&m_pZmqContext);
	zmq_msg_close(&m_zmqMsg);

	return true;
}

void CGatewayServer::s_dump()
{
	int rc = 0;

	zmq_msg_t message1;
	zmq_msg_t message2;
	rc = zmq_msg_init(&message1);
	rc = zmq_msg_init(&message2);

	//  Dump the message as text or binary

	int size = zmq_msg_recv(&message1, m_pZmqSocket, 0);
	size = zmq_msg_recv(&message2, m_pZmqSocket, 0);

	zmq_msg_send(&message1, m_pZmqSocket, ZMQ_SNDMORE);
	zmq_msg_send(&message2, m_pZmqSocket, 0);

	zmq_msg_close(&message1);
	zmq_msg_close(&message2);
}

void CGatewayServer::MonitorFunction()
{
	static char addr[1025];
	int rc;
	printf("starting monitor...\n");
	void *s = zmq_socket(m_pZmqContext, ZMQ_PAIR);
	rc = zmq_connect(s, "inproc://monitor.rep");
	zmq_event_t zmqEvent;
	while (!read_msg(s, &zmqEvent, addr))
	{
		printf("monitor socket event %d\n", zmqEvent.event);
		printf("monitor socket value %d\n", zmqEvent.value);
		printf("monitor socket address %s\n", addr);

	}
	zmq_close(s);
}

int CGatewayServer::read_msg(void* s, zmq_event_t* event, char* ep)
{
	int rc;
	zmq_msg_t msg1;  // binary part
	zmq_msg_init(&msg1);
	zmq_msg_t msg2;  //  address part
	zmq_msg_init(&msg2);
	rc = zmq_msg_recv(&msg1, s, 0);
	if (rc == -1 && zmq_errno() == ETERM)
		return 1;

	rc = zmq_msg_recv(&msg2, s, 0);
	if (rc == -1 && zmq_errno() == ETERM)
		return 1;

	// copy binary data to event struct
	const char* data = (char*)zmq_msg_data(&msg1);
	memcpy(&(event->event), data, sizeof(event->event));
	memcpy(&(event->value), data + sizeof(event->event), sizeof(event->value));
	// copy address part
	const size_t len = zmq_msg_size(&msg2);
	ep = (char*)memcpy(ep, zmq_msg_data(&msg2), len);
	*(ep + len) = 0;
	return 0;
}