#include "Proactor.h"
#include "AsynIoDevice.h"
#include "AsynIoFrame.h"

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

#ifdef _WINDOWS
	HANDLE pHandle = ::CreateIoCompletionPort(pDevice->GetHandle(), m_hCompletePort, (ULONG_PTR)pDevice, m_nConcurrentThreadsNum);
	return pHandle == m_hCompletePort;
#else

#endif // _WINDOWS
}

bool Proactor::HandleEvents(unsigned long nTimeOut)
{
#ifdef _WINDOWS
	DWORD dwTransferredBytes = 0;
	ULONG_PTR completeKey = 0;
	LPOVERLAPPED overLapped = 0;

	BOOL result = ::GetQueuedCompletionStatus(m_hCompletePort, &dwTransferredBytes, &completeKey, &overLapped, nTimeOut);

	// 操作成功
	if (TRUE == result)
	{
		IAsynIoResult *io_result = (IAsynIoResult *)overLapped;

		if (!io_result->IsSimulation())
		{
			io_result->SetTransferredBytes(dwTransferredBytes);
			io_result->SetErrorCode(ERROR_SUCCESS);
		}

		io_result->Complete();
		return true;
	}
	else
	{
		int error_code = ::GetLastError();

		// 操作完成，但是出现了错误
		if (overLapped)
		{
			IAsynIoResult * io_result = (IAsynIoResult *)overLapped;

			io_result->SetTransferredBytes(dwTransferredBytes);
			io_result->SetErrorCode(error_code);
			io_result->Complete();
			return true;

		}
		else
		{
			switch (error_code)
			{
			case WAIT_TIMEOUT:
				return true;

			case ERROR_SUCCESS:
				// Calling GetQueuedCompletionStatus with timeout value 0
				// returns FALSE with extended errno "ERROR_SUCCESS" errno =
				// ETIME; ?? I don't know if this has to be done !!
				return true;
			default:
				return false;
			}
		}
	}
#else
	
#endif // _WINDOWS
}

void Proactor::HandleEventsLoop()
{

}

bool Proactor::PostCompletion(IAsynIoResult * pResult)
{
	pResult->SetSimulation(true);
	return ::PostQueuedCompletionStatus(m_hCompletePort, 0, 0, pResult) == TRUE;
}
