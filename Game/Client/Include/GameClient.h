#ifndef _GAMECLIENT_H
#define _GAMECLIENT_H

#include "Singleton.hpp"
#include "IThread.h"
#include "Log.h"

#ifdef __cplusplus
extern "C"
{
#include "zmq.h"
}
#endif

class CGameClient : public IThread,
	public CSingleton<CGameClient>
{
public:
	~CGameClient();
	bool InitClient();
	bool Start();

	void SendMsg(const char *pMsg, size_t nLength);

private:
	void ProcessLogic();
	bool BeginStop();

private:

	void *m_pZmqContext = nullptr;
	void *m_pZmqSocket = nullptr;
	zmq_msg_t m_zmqMsg;
};

#endif // _GAMECLIENT_H