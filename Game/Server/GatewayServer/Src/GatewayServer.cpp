#include "GatewayServer.h"
#include "Log.h"
#include <iostream>

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
		SaveAssertLog("m_pZmqSocket 失败");
		return false;
	}

	int rc = zmq_bind(m_pZmqSocket, "tcp://*:5555");
	if (rc < 0)
	{
		SaveAssertLog("error in zmq_bind, return:%d, error msg:%s", rc, zmq_strerror(errno));
		return false;
	}

	rc = zmq_socket_monitor(m_pZmqSocket, "inproc://monitor.rep", ZMQ_EVENT_ACCEPTED | ZMQ_EVENT_DISCONNECTED);
	if (rc < 0)
	{
		SaveAssertLog("error in zmq_socket_monitor, return:%d, error msg:%s", rc, zmq_strerror(errno));
		return false;
	}

	m_pMonitorSocket = zmq_socket(m_pZmqContext, ZMQ_PAIR);
	if (nullptr == m_pMonitorSocket)
	{
		SaveAssertLog("m_pMonitorSocket 失败");
		return false;
	}

	rc = zmq_connect(m_pMonitorSocket, "inproc://monitor.rep");
	if (rc < 0)
	{
		SaveAssertLog("error in zmq_socket_monitor, return:%d, error msg:%s", rc, zmq_strerror(errno));
		return false;
	}

	m_tPolloer[0].socket = m_pMonitorSocket;
	m_tPolloer[0].fd = 0;
	m_tPolloer[0].events = ZMQ_POLLIN;

	m_tPolloer[1].socket = m_pZmqSocket;
	m_tPolloer[1].fd = 0;
	m_tPolloer[1].events = ZMQ_POLLIN;

	/*CTimerAxist::GetInstance()->SetTimer(1, 1000, this, TIMER_INFINITY_CALL, "测试1");
	CTimerAxist::GetInstance()->SetTimer(2, 1000, this, TIMER_INFINITY_CALL, "测试2");
	CTimerAxist::GetInstance()->SetTimer(3, 3000, this, TIMER_INFINITY_CALL, "测试3");
	CTimerAxist::GetInstance()->SetTimer(4, 7000, this, TIMER_INFINITY_CALL, "测试4");*/

	return true;
}

bool CGatewayServer::Start()
{
	return IThread::Start();
}

void CGatewayServer::OnTimer(uint32_t nTimerID)
{
	switch (nTimerID)
	{
	case 1:
	{
		std::cout << "定时器触发1" << std::endl;
		CTimerAxist::GetInstance()->KillTimer(1, this);
	}
	break;
	case 2:
	{
		std::cout << "定时器触发2" << std::endl;
		CTimerAxist::GetInstance()->KillTimer(2, this);
	}
	break;
	case 3:
	{
		std::cout << "定时器触发3" << std::endl;
		CTimerAxist::GetInstance()->KillTimer(3, this);
	}
	break;
	case 4:
	{
		std::cout << "定时器触发4" << std::endl;
	}
	break;
	default:
		break;
	}
}

void CGatewayServer::ProcessLogic()
{
	CTimerAxist::GetInstance()->CheckTimer();

	int nRet = zmq_poll(m_tPolloer, 2, -1);
	if (0 == nRet)
		return;

	if (m_tPolloer[0].revents & ZMQ_POLLIN)
	{ 
		zmq_msg_t message;
		zmq_msg_init(&message);

		int size = zmq_msg_recv(&message, m_pMonitorSocket, 0);
		printf("-------------event_begin-------------\n");
		uint8_t* pMsg = (uint8_t*)zmq_msg_data(&message);
		zmq_event_t event_t;
		memcpy(&event_t.eventID, pMsg, sizeof(event_t.eventID));
		memcpy(&event_t.eventValue, pMsg + sizeof(event_t.eventID), sizeof(event_t.eventValue));
		printf("event_id:%x\n", event_t.eventID);
		printf("event_value:%d\n", event_t.eventValue);

		char szAddress[128] = { 0 };
		size = zmq_msg_recv(&message, m_pMonitorSocket, 0);
		memcpy(szAddress, zmq_msg_data(&message), size);
		printf("event_address:%s\n", szAddress);

		printf("-------------event_end-------------\n");
	}
	
	if (m_tPolloer[1].revents & ZMQ_POLLIN)
	{
		zmq_msg_t msgAddress;
		int rc = zmq_msg_init(&msgAddress);

		int size = zmq_msg_recv(&msgAddress, m_pZmqSocket, 0);
		printf("-------------recv_begin-------------\n");
		// uint32_t nMsgFd = zmq_msg_get(&msgAddress, ZMQ_SRCFD);
		// printf("msg_from:%d\n", nMsgFd);

		zmq_msg_t message;
		rc = zmq_msg_init(&message);
		size = zmq_msg_recv(&message, m_pZmqSocket, 0);

		uint32_t nMsgFd = zmq_msg_get(&message, ZMQ_SRCFD);
		printf("msg_from:%d\n", nMsgFd);

		char szMsg[1024] = { 0 };
		memcpy(szMsg, zmq_msg_data(&message), size);
		printf("msg:%s\n", szMsg);

		zmq_msg_send(&msgAddress, m_pZmqSocket, ZMQ_SNDMORE);
		zmq_msg_send(&message, m_pZmqSocket, 0);

		zmq_msg_close(&msgAddress);
		zmq_msg_close(&message);

		printf("-------------recv_end-------------\n");
	}
}

bool CGatewayServer::BeginStop()
{
	zmq_close(&m_pZmqSocket);
	zmq_close(&m_pMonitorSocket);
	zmq_term(&m_pZmqContext);

	return true;
}
