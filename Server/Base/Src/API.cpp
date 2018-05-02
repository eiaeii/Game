#include "API.h"

void *g_ServerProvider = nullptr;

void SetServerProvider(void *pServer)
{
	g_ServerProvider = pServer;
}

void* GetServerProvider()
{
	return g_ServerProvider;
}
