#ifndef ASYNIOSOCKETDEVICE_H
#include "AsynIoDevice.h"

struct IAsynIoOperation_Reader;
struct IAsynIoOperation_Writer;
struct IAsynIoOperation_Acceptor;
struct IAsynIoOperation_Connector;
struct IAsynIoResult;

class AsynIoSccketDevice : public IAsynIoDevice
{
public:
	AsynIoSccketDevice();
	~AsynIoSccketDevice();

	HANDLE GetHandle() const override;

	IAsynIoOperation_Reader* GetReader() const override;

	IAsynIoOperation_Writer* GetWriter() const override;

	IAsynIoOperation_Acceptor* GetAcceptor() const override;

	IAsynIoOperation_Connector* GetConnector() const override;

	IAsynIoResult* GreateAsynIoResult(AsynIoType nIoType = AsynIoType::AsynIoType_Unknow, IAsynIoHandler* pHandler = nullptr) override;

	void DeleteAsynIoResult(IAsynIoResult *pResult) override;

	void CloseDevice() override;

	void Release(bool bForce = false) override;

private:

#ifdef _WINDOWS
	SOCKET m_hSocket;
#else

#endif // _WINDOWS

	
};

#endif // !ASYNIOSOCKETDEVICE_H
