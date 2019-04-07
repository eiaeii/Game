#ifndef _PROACTOR_H
#define _PROACTOR_H

#include "CommonDefine.h"
#include "IProactor.h"

struct IAsynIoDevice;
struct IAsynIoResult;

class Proactor : public IProactor
{
	bool Create(unsigned long nThreadNum = 0) override;

	void Close() override;

	bool ResgisterAsynIoDevice(IAsynIoDevice *pDevice) override;

	bool HandleEvents(unsigned long nTimeOut = INFINITE) override;

	void HandleEventsLoop() override;

	bool PostCompletion(IAsynIoResult *pResult) override;

private:

	HANDLE  m_hCompletePort = nullptr; // 完成端口句柄
	unsigned long m_nConcurrentThreadsNum = 0; // 并发工作线程序数
};

#endif // _PROACTOR_H