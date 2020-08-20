#ifndef _GATEWAYSERVER_H
#define _GATEWAYSERVER_H

#include "Singleton.hpp"
#include "IThread.h"
#include "Log.h"
#include "zmq.h"
#include "TimerAxis.hpp"

#pragma pack(1)
struct zmq_event_t
{
	uint16_t eventID = 0;
	uint32_t eventValue = 0;
};
#pragma pack()

class CGatewayServer : public IThread, public ITimerHandler,
	public CSingleton<CGatewayServer>
{
public:
	~CGatewayServer();
	bool InitServer();
	bool Start();
	void OnTimer(uint32_t nTimerID);

private:
	void ProcessLogic();
	bool BeginStop();
	void MonitorFunction();
private:

	void *m_pZmqContext = nullptr;
	void *m_pZmqSocket = nullptr;
	zmq_msg_t m_zmqMsg;
};

#endif // _GATEWAYSERVER_H