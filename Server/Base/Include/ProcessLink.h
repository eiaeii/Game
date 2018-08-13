#ifndef _PROCESSLINK_H
#define _PROCESSLINK_H

#include "Lock.h"
class CLock;

class CProcessLink
{
public:
	virtual ~CProcessLink() {};

	void LinkBefore(CProcessLink *pObj);

private:
	void SetNext(CProcessLink *pNext);
	CProcessLink* GetNext();
	void SetPrev(CProcessLink *pPrev);
	CProcessLink* GetPrev();

private:
	CProcessLink *m_pPrev = nullptr;
	CProcessLink *m_pNext = nullptr;
};

#endif // !_PROCESSLINK_H