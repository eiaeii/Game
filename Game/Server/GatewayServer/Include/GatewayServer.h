#ifndef _GATEWAYSERVER_H
#define _GATEWAYSERVER_H

#include "Singleton.hpp"
#include "IThread.h"

class GatewayServer : public IThread,
	public CSingleton<GatewayServer>
{
public:
	~GatewayServer();
	bool InitServer();
	bool Start();

private:
	void ProcessLogic();
	bool BeginStop();
};

#endif // _GATEWAYSERVER_H