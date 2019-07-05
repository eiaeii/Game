#include "IThread.h"

void IThread::BeginRun()
{
	srand((unsigned)time(NULL));
}

bool IThread::Start()
{
	if (ThreadStatus::THREAD_STATUS_READY != m_btStatus)
		return false;

	std::thread(ThreadFunction, this).detach();

	return true;
}

bool IThread::Run()
{
	if (ThreadStatus::THREAD_STATUS_RUNNING == m_btStatus)
	{
		ProcessLogic();
		return true;
	}

	return false;
}

bool IThread::Stop()
{
	// ÉèÖÃÏß³ÌÍ£Ö¹
	this->SetStatus(ThreadStatus::THREAD_STATUS_EXITING);

	while (m_btStatus != ThreadStatus::THREAD_STATUS_EXIT)
	{
		Sleep();
	}

	this->SetStatus(ThreadStatus::THREAD_STATUS_READY);

	return true;
}

void IThread::ThreadFunction(void *pParm)
{
	IThread *pThread = static_cast<IThread*>(pParm);
	if (nullptr == pThread)
		return;

	pThread->BeginRun();

	pThread->SetStatus(ThreadStatus::THREAD_STATUS_RUNNING);

	while (true)
	{
		if (pThread->Run())
			pThread->Sleep();
		else
		{
			pThread->BeginStop();
			break;
		}
	}

	pThread->SetStatus(ThreadStatus::THREAD_STATUS_EXIT);

	return;
}