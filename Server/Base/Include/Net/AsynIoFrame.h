#ifndef ASYNIOFRAME_H
#define ASYNIOFRAME_H

// 最大Buff数目
constexpr int MAX_ASYN_IO_BUFF_COUNT = 128;

struct IAsynIoHandler;
struct IAsynIoBuffer;
struct IAsynIoDevice;

enum  class AsynIoType : unsigned char
{
	AsynIoType_Unknow = 0,
	AsynIoType_Read,
	AsynIoType_Write,
	AsynIoType_Accept,
	AsynIoType_Connect,
	AsynIoType_ReadFrom,
	AsynIoType_WriteTo,
	AsynIoType_Close,
	AsynIoType_DisConnect,
};

struct IAsynIoHandler
{
	virtual void HandleIoComplete(AsynIoResult* pResult) = 0;

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
	virtual void SetAsynIoHandler(AsynIoHandler* handler) = 0;

	virtual void Complete() = 0;

	virtual void PrepareBuffer(unsigned long nLen) = 0;

	virtual void SetAsynIoBuffer(AsynIoBuffer* pBufferList, unsigned long nBufferCount) = 0;

	virtual AsynIoBuffer* GetAsynIoBufferList() = 0;

	virtual unsigned long GetAsynIoBufferCount() const = 0;

	virtual unsigned long GetTransferredBytes() const = 0;

	virtual void  SetTransferredBytes(unsigned long bytes) = 0;

	virtual bool IsSuccess() const = 0;

	virtual unsigned long GetErrorCode() const = 0;

	virtual void SetErrorCode(unsigned long nCode) = 0;

	virtual AsynIoDevice* GetAsynIoDevice() const = 0;

	virtual void SetAsynIoDevice(AsynIoDevice* pDevice) = 0;

	virtual AsynIoType GetAsynIoType() const = 0;

	virtual SetAsynIoType(AsynIoType btType) = 0;

	virtual void Release() = 0;
};

struct AsynIoBuffer
{
	unsigned long len;
	unsigned char *buf;
};

#endif // ASYNIOFRAME_H