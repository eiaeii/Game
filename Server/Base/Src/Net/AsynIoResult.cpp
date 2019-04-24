#include "AsynIoResult.h"
#include "AsynIoDevice.h"
#include <memory.h>
#include "MT_MemPool.h"
#include "Log.h"

AsynIoResult::AsynIoResult()
{
#ifdef _WINDOWS
	Internal = InternalHigh = 0;
	Pointer = 0;
	hEvent = 0;
#else

#endif // _WINDOWS
}

AsynIoResult::AsynIoResult(AsynIoType nType, IAsynIoHandler * pHandler)
{
	m_Type = nType;
#ifdef _WINDOWS
	Internal = InternalHigh = 0;
	Pointer = 0;
	hEvent = 0;
#else

#endif // _WINDOWS
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
	for (unsigned long i = 0; i < m_dwBuffCount; ++i)
	{
		MT_Free(m_BuffList[i].buf);
	}

	if (0 == nLen)
	{
		m_dwBuffCount = 0;
		return;
	}

	m_dwBuffCount = 1;
	m_BuffList[0].buf = (unsigned char*)MT_Alloc(nLen);
	m_BuffList[0].len = nLen;
}

void AsynIoResult::SetAsynIoBuffer(AsynIoBuffer* pBufferList, unsigned long nBufferCount)
{
	assert(nBufferCount < MAX_ASYN_IO_BUFF_COUNT);
	
	for (unsigned long i = 0; i < nBufferCount; ++i)
	{
		m_BuffList[i] = pBufferList[i];
	}

	m_dwBuffCount = nBufferCount;
}

AsynIoBuffer* AsynIoResult::GetAsynIoBufferList()
{
	return 0 == m_dwBuffCount ? nullptr : m_BuffList;
}

unsigned long AsynIoResult::GetAsynIoBufferCount() const
{
	return m_dwBuffCount;
}

unsigned long AsynIoResult::GetTransferredBytes() const
{
	return m_dwTransferredBytes;
}

void  AsynIoResult::SetTransferredBytes(unsigned long bytes)
{
	m_dwTransferredBytes = bytes;
}

bool AsynIoResult::IsSuccess() const
{
	// todo return m_dwErrorCode == ERROR_SUCCESS;
	return false;
}

unsigned long AsynIoResult::GetErrorCode() const
{
	return m_dwErrorCode;
}

void AsynIoResult::SetErrorCode(unsigned long nCode)
{
	m_dwErrorCode = nCode;
}

IAsynIoDevice* AsynIoResult::GetAsynIoDevice() const
{
	return m_pDevice;
}

void AsynIoResult::SetAsynIoDevice(IAsynIoDevice* pDevice)
{
	if (m_pDevice)
	{
		SaveAssertLog("AsynIoResult::SetAsynIoDevice m_pDevice != nullptr");
	}

	m_pDevice = pDevice;

	if (m_pDevice)
	{
		m_pDevice->AddRef();
	}
}

AsynIoType AsynIoResult::GetAsynIoType() const
{
	return m_Type;
}

void AsynIoResult::SetAsynIoType(AsynIoType btType)
{
	m_Type = btType;
}

bool AsynIoResult::IsSimulation()
{
	return m_bSimulation;
}

void AsynIoResult::SetSimulation(bool bSimulation)
{
	m_bSimulation = bSimulation;
}

void AsynIoResult::Release()
{
	delete this;
}