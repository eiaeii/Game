#ifndef ASYNIOFRAME_H
#define ASYNIOFRAME_H

#include "NetDefine.h"

struct IAsynIoHandler;
struct IAsynIoBuffer;
struct IAsynIoDevice;
struct IAsynIoResult;

struct IAsynIoHandler
{
	virtual void HandleIoComplete(IAsynIoResult* pResult) = 0;

	virtual void ReleaseRef() = 0;

	virtual void AddRef() = 0;
};

#ifdef _WINDOWS
	#include <Windows.h>
#else
	#include <sys/epoll.h>
#endif // _WINDOWS

#ifdef _WINDOWS
struct IAsynIoResult : public OVERLAPPED
#else
struct IAsynIoResult : public epoll_event
#endif // _WINDOWS
{
	virtual void SetAsynIoHandler(IAsynIoHandler* pHandler) = 0;

	virtual void Complete() = 0;

	virtual void PrepareBuffer(unsigned long nLen) = 0;

	virtual void SetAsynIoBuffer(IAsynIoBuffer* pBufferList, unsigned long nBufferCount) = 0;

	virtual IAsynIoBuffer* GetAsynIoBufferList() = 0;

	virtual unsigned long GetAsynIoBufferCount() const = 0;

	virtual unsigned long GetTransferredBytes() const = 0;

	virtual void  SetTransferredBytes(unsigned long bytes) = 0;

	virtual bool IsSuccess() const = 0;

	virtual unsigned long GetErrorCode() const = 0;

	virtual void SetErrorCode(unsigned long nCode) = 0;

	virtual IAsynIoDevice* GetAsynIoDevice() const = 0;

	virtual void SetAsynIoDevice(IAsynIoDevice* pDevice) = 0;

	virtual AsynIoType GetAsynIoType() const = 0;

	virtual void SetAsynIoType(AsynIoType btType) = 0;

	virtual bool IsSimulation() = 0;

	virtual void SetSimulation(bool bSimulation) = 0;

	virtual void Release() = 0;
};

struct AsynIoBuffer
{
	unsigned long len = 0;
	unsigned char *buf = nullptr;
};

#endif // ASYNIOFRAME_H