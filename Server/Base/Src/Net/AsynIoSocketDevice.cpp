#include "AsynIoSocketDevice.h"

AsynIoSccketDevice::AsynIoSccketDevice()
{

}

AsynIoSccketDevice::~AsynIoSccketDevice()
{

}

HANDLE AsynIoSccketDevice::GetHandle() const
{
	return HANDLE();
}

IAsynIoOperation_Reader * AsynIoSccketDevice::GetReader() const
{
	return nullptr;
}

IAsynIoOperation_Writer * AsynIoSccketDevice::GetWriter() const
{
	return nullptr;
}

IAsynIoOperation_Acceptor * AsynIoSccketDevice::GetAcceptor() const
{
	return nullptr;
}

IAsynIoOperation_Connector * AsynIoSccketDevice::GetConnector() const
{
	return nullptr;
}

IAsynIoResult * AsynIoSccketDevice::GreateAsynIoResult(AsynIoType nIoType, IAsynIoHandler * pHandler)
{
	nIoType;
	pHandler;
	return nullptr;
}

void AsynIoSccketDevice::DeleteAsynIoResult(IAsynIoResult * pResult)
{
	pResult;
}

void AsynIoSccketDevice::CloseDevice()
{

}

void AsynIoSccketDevice::Release(bool bForce)
{
	bForce;
}
