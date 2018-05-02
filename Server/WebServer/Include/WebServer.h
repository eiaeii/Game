#ifndef _WEBSERVER_H
#define _WEBSERVER_H

#include "Singleton.hpp"
#include "IThread.h"
#include "IEngineProvider.h"

class CWebServer : public IThread,
	public CSingleton<CWebServer>
{
public:
	~CWebServer();
	bool InitServer();
	bool Start();

private:
	void ProcessLogic();
	bool BeginStop();
};

#endif // _WEBSERVER_H