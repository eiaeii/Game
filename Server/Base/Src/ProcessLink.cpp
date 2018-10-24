#include "Lock.h"
#include "ProcessLink.h"

inline void CProcessLink::SetNext(CProcessLink *pNext)
{
	if (nullptr == pNext || pNext == this)
		return;

	this->m_pNext = pNext;
}

inline CProcessLink* CProcessLink::GetNext()
{
	return m_pNext;
}

inline void CProcessLink::SetPrev(CProcessLink *pPrev)
{
	if (nullptr == pPrev || pPrev == this)
		return;

	this->m_pPrev = pPrev;
}

inline CProcessLink* CProcessLink::GetPrev()
{
	return m_pPrev;
}

inline void CProcessLink::LinkBefore(CProcessLink * pObj)
{
	if (nullptr == pObj || pObj == this)
		return;

	if (nullptr == pObj->GetPrev())
	{
		pObj->SetNext(this);
		this->SetPrev(pObj);
	}
	else
	{
		pObj->GetPrev()->SetNext(this);
		this->SetNext(pObj);

		this->SetPrev(pObj->GetPrev());
		pObj->SetPrev(this);
	}
}
