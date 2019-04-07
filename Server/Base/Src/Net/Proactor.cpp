#include "Proactor.h"
#include "AsynIoDevice.h"

bool Proactor::Create(unsigned long nThreadNum)
{
	bool bResult = false;
	m_nConcurrentThreadsNum = nThreadNum;
#ifdef _WINDOWS
	m_hCompletePort = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, nThreadNum);
	bResult = m_hCompletePort != nullptr;
#else

#endif

	return bResult;
}

void Proactor::Close()
{
	if (nullptr != m_hCompletePort)
	{
#ifdef _WINDOWS
		::CloseHandle(m_hCompletePort);
#endif // _WINDOWS

	}

	m_hCompletePort = nullptr;
}

bool Proactor::ResgisterAsynIoDevice(IAsynIoDevice * pDevice)
{
	if (nullptr == pDevice)
		return false;

	bool bResult = false;

#ifdef _WINDOWS
	HANDLE completeHandle = ::CreateIoCompletionPort(pDevice->GetHandle(), m_hCompletePort, (ULONG_PTR)pDevice, m_nConcurrentThreadsNum);
	bResult = completeHandle == m_hCompletePort;
#else

#endif // _WINDOWS

	return bResult;
}

bool Proactor::HandleEvents(unsigned long nTimeOut)
{
	nTimeOut;
	return false;
}

void Proactor::HandleEventsLoop()
{
}

bool Proactor::PostCompletion(IAsynIoResult * pResult)
{
	pResult;
	return false;
}
