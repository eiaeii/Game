#include <atomic>

#ifndef _Lock_HPP
#define _Lock_HPP

class CLock
{
public:
	inline void Lock()
	{
		while (m_flag.test_and_set());
	}

	inline void UnLock()
	{
		m_flag.clear();
	}

private:
	std::atomic_flag m_flag = ATOMIC_FLAG_INIT;
};


class CAutoLock
{
public:

	explicit CAutoLock(CLock &rLock)
	{
		m_pLock = &rLock;
		m_pLock->Lock();
	}

	~CAutoLock()
	{
		m_pLock->UnLock();
	}

private:
	CLock *m_pLock;
	CAutoLock();
};


#endif // _Lock_HPP

