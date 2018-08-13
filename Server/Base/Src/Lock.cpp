#include "Lock.h"

inline void CLock::Lock()
{
	m_mutex.lock();
}

inline void CLock::UnLock()
{
	m_mutex.unlock();
}


CAutoLock::CAutoLock(CLock &rLock)
{
	m_pLock = &rLock;
	m_pLock->Lock();
}

CAutoLock::~CAutoLock()
{
	m_pLock->UnLock();
}
