#ifndef ASYNIODEVICE_H
#define ASYNIODEVICE_H

#include "CommonDefine.h"
#include "NetDefine.h"

struct IAsynIoResult;
struct IAsynIoHandler;
struct IAsynIoOperation_Reader;
struct IAsynIoOperation_Writer;
struct IAsynIoOperation_Acceptor;
struct IAsynIoOperation_Connector;

struct IAsynIoDevice
{
	virtual HANDLE GetHandle() const = 0;

	virtual IAsynIoOperation_Reader* GetReader() const = 0;

	virtual IAsynIoOperation_Writer* GetWriter() const = 0;

	virtual IAsynIoOperation_Acceptor* GetAcceptor() const = 0;

	virtual IAsynIoOperation_Connector* GetConnector() const = 0;

	virtual IAsynIoResult* GreateAsynIoResult(AsynIoType nIoType = AsynIoType::AsynIoType_Unknow, IAsynIoHandler* pHandler = nullptr) = 0;

	virtual void DeleteAsynIoResult(IAsynIoResult *pResult) = 0;

	virtual void CloseDevice() = 0;

	virtual void Release(bool bForce = false) = 0;

	virtual void AddRef() = 0;
};

#endif // ASYNIODEVICE_H