#ifndef _Lock_HP
#define _Lock_HP
#include <atomic>

class CLock
{
public:
	void Lock();

	void UnLock();
	
private:
	std::atomic_flag m_flag = ATOMIC_FLAG_INIT;
};


class CAutoLock
{
public:

	explicit CAutoLock(CLock &rLock);

	~CAutoLock();

private:
	CLock *m_pLock = nullptr;
	CAutoLock() = delete;
};


#endif // _Lock_H

