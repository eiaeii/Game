#include "Lock.h"

void CLock::Lock()
{
	while (m_flag.test_and_set());
}

void CLock::UnLock()
{
	m_flag.clear();
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
