#ifndef _IPROACTOR_H
#define _IPROACTOR_H

#include "CommonDefine.h"

struct IAsynIoDevice;
struct IAsynIoResult;

struct IProactor
{
	virtual bool Create(unsigned long nThreadNum = 0) = 0;

	virtual void Close() = 0;

	virtual bool ResgisterAsynIoDevice(IAsynIoDevice *pDevice) = 0;

	virtual bool HandleEvents(unsigned long nTimeOut = 0xFFFFFFFF) = 0;

	virtual void HandleEventsLoop() = 0;

	virtual bool PostCompletion(IAsynIoResult *pResult) = 0;
};

#endif // _IPROACTOR_H