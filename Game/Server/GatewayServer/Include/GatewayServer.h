#ifndef _GATEWAYSERVER_H
#define _GATEWAYSERVER_H

#include "Singleton.hpp"
#include "IThread.h"
#include "Log.h"
#include "zmq.h"

struct zmq_event_t
{
	unsigned short event = 0;
	int value = 0;
};

class CGatewayServer : public IThread,
	public CSingleton<CGatewayServer>
{
public:
	~CGatewayServer();
	bool InitServer();
	bool Start();
	void s_dump();

private:
	void ProcessLogic();
	bool BeginStop();
	void MonitorFunction();
	int read_msg(void* s, zmq_event_t* event, char* ep);
private:

	void *m_pZmqContext = nullptr;
	void *m_pZmqSocket = nullptr;
	zmq_msg_t m_zmqMsg;
};

#endif // _GATEWAYSERVER_H