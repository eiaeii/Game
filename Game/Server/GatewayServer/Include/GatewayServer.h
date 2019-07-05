#ifndef _GATEWAYSERVER_H
#define _GATEWAYSERVER_H

#include "Singleton.hpp"
#include "IThread.h"
#include "Log.h"

class CGatewayServer : public IThread,
	public CSingleton<CGatewayServer>
{
public:
	~CGatewayServer();
	bool InitServer();
	bool Start();

private:
	void ProcessLogic();
	bool BeginStop();
};

#endif // _GATEWAYSERVER_H