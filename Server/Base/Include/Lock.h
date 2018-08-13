#ifndef _Lock_HP
#define _Lock_HP

#include <mutex>

class CLock
{
public:
	void Lock();

	void UnLock();

private:
	std::mutex m_mutex;
};


class CAutoLock
{
public:

	explicit CAutoLock(CLock &rLock);

	~CAutoLock();

private:
	CLock * m_pLock = nullptr;
	CAutoLock() = delete;
};

#endif // _Lock_H

