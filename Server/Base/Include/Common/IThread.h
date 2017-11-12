#include <thread>
#include <chrono>
#include <atomic>

#ifndef _Thread_HPP
#define _Thread_HPP


class IThread
{
	enum class ThreadStatus : unsigned char
	{
		THREAD_STATUS_READY = 1,	// ��ǰ�̴߳���׼��״̬
		THREAD_STATUS_RUNNING,	// ��������״̬
		THREAD_STATUS_SUSPEND,	// �̹߳�����ͣ
		THREAD_STATUS_EXITING,	// �߳������˳�
		THREAD_STATUS_EXIT		// �Ѿ��˳� 
	};

public:
	
	ThreadStatus GetStatus()
	{
		return m_btStatus;
	}

	void BeginRun();

	virtual ~IThread() {}

	virtual bool Start();

	bool Run();

	bool Stop();
	
private:
	virtual void ProcessLogic() = 0;
	virtual bool BeginStop() = 0;

	void Sleep()
	{
		std::this_thread::sleep_for(m_sleepTime);
	}

	static void ThreadFunction(void *pParm);
	
	void SetStatus(ThreadStatus btStatus)
	{
		m_btStatus = btStatus;
	}

private:
	std::atomic<ThreadStatus> m_btStatus = ThreadStatus::THREAD_STATUS_READY;
	std::chrono::milliseconds m_sleepTime = std::chrono::milliseconds(1);
};

#endif // _Thread_HPP

