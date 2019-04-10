#include "AsynIoResult.h"
#include "AsynIoDevice.h"

AsynIoResult::AsynIoResult()
{
	Internal = InternalHigh = 0;
	Pointer = 0;
	hEvent = 0;
}

AsynIoResult::AsynIoResult(AsynIoType nType, IAsynIoHandler * pHandler)
{
	m_Type = nType;
	Internal = InternalHigh = 0;
	Pointer = 0;
	hEvent = 0;
	pHandler->AddRef();
}

void AsynIoResult::SetAsynIoHandler(IAsynIoHandler* pHandler)
{
	if (nullptr == pHandler)
		return;

	m_pHandler = pHandler;
	m_pHandler->AddRef();
}

void AsynIoResult::Complete()
{
	IAsynIoHandler * pHandler = m_pHandler;
	IAsynIoDevice  * pDevice = m_pDevice;

	if (pHandler && pDevice)
	{
		pHandler->HandleIoComplete(this);

		pDevice->Release();
		pHandler->ReleaseRef();
	}
}

void AsynIoResult::PrepareBuffer(unsigned long nLen)
{
	nLen;
}

void AsynIoResult::SetAsynIoBuffer(IAsynIoBuffer* pBufferList, unsigned long nBufferCount)
{
	pBufferList;
	nBufferCount;
}

IAsynIoBuffer* AsynIoResult::GetAsynIoBufferList()
{
	return nullptr;
}

unsigned long AsynIoResult::GetAsynIoBufferCount() const
{
	return 0;
}

unsigned long AsynIoResult::GetTransferredBytes() const
{
	return 0;
}

void  AsynIoResult::SetTransferredBytes(unsigned long bytes)
{
	bytes;
}

bool AsynIoResult::IsSuccess() const
{
	return false;
}

unsigned long AsynIoResult::GetErrorCode() const
{
	return 0;
}

void AsynIoResult::SetErrorCode(unsigned long nCode)
{
	nCode;
}

IAsynIoDevice* AsynIoResult::GetAsynIoDevice() const
{
	return nullptr;
}

void AsynIoResult::SetAsynIoDevice(IAsynIoDevice* pDevice)
{
	pDevice;
}

AsynIoType AsynIoResult::GetAsynIoType() const
{
	return AsynIoType::AsynIoType_Unknow;
}

void AsynIoResult::SetAsynIoType(AsynIoType btType)
{
	btType;
}

bool AsynIoResult::IsSimulation()
{
	return false;
}

void AsynIoResult::SetSimulation(bool bSimulation)
{
	bSimulation;
}

void AsynIoResult::Release()
{

}