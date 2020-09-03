#include "TimerAxis.hpp"
#include <chrono>
#include "Log.h"

ITimerHandler::~ITimerHandler()
{
	CTimerAxist::GetInstance()->KillTimer(static_cast<ITimerHandler*>(this));
}

CTimerAxist::CTimerAxist()
{
	// ��ʱ���Ữ��ΪN���̶�
	m_vecTimerAxis.resize((TIME_AXIS_LENGTH + TIME_GRID_INTERVAL - 1) / TIME_GRID_INTERVAL);
	m_InitTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
	m_nLastCheckTick = m_InitTime;
	m_nCurRunTime = m_InitTime;
	m_nTimerCount = 0;
}

CTimerAxist::~CTimerAxist()
{
	for (auto &tTimerList : m_vecTimerAxis)
	{
		for (auto& pOneTimer : tTimerList)
		{
			if (pOneTimer)
			{
				KillTimer(pOneTimer->nTimerID, pOneTimer->pHandler);
			}
		}
	}

	m_vecTimerAxis.clear();
}

bool CTimerAxist::SetTimer(uint32_t nTimerID, uint32_t nInterval, ITimerHandler* pHandler, uint32_t nCallTimes, const char* pszDebugInfo)
{
	if (nullptr == pHandler)
		return false;

	nInterval = nInterval == 0 ? 1 : nInterval;
	void** ppTimerInfo = pHandler->GetTimerInfo();
	if (nullptr == ppTimerInfo)
		return false;

	TIMER_INFO* pTimerInfo = *(TIMER_INFO**)ppTimerInfo;
	if (nullptr == pTimerInfo)
	{
		pTimerInfo = new TIMER_INFO;
		*ppTimerInfo = pTimerInfo;
	}

	// ����Ƿ���������timer
	for (const auto& itr : *pTimerInfo)
	{
		if (itr.nTimerID == nTimerID)
			return false;
	}

	Timer tOneTimer;
	tOneTimer.nTimerID = nTimerID;
	tOneTimer.nInterval = nInterval;
	tOneTimer.nCallTimes = nCallTimes;
	tOneTimer.nLastCallTick = m_nLastCheckTick;
	tOneTimer.pHandler = pHandler;
	tOneTimer.nGridIndex = uint32_t(((m_nLastCheckTick - m_InitTime + nInterval) / TIME_GRID_INTERVAL) % m_vecTimerAxis.size());
	if (pszDebugInfo)
		tOneTimer.strDebugInfo = pszDebugInfo;
	pTimerInfo->push_back(tOneTimer);

	// ��Timer��ӵ���Ӧ��ʱ��̶�
	Timer& timerRef = pTimerInfo->back();
	Timer* timerPtr = &timerRef;
	m_vecTimerAxis[tOneTimer.nGridIndex].push_back(timerPtr);
	timerPtr->itrPos = --m_vecTimerAxis[tOneTimer.nGridIndex].end();
	++m_nTimerCount;

	return true;
}

bool CTimerAxist::CTimerAxist::KillTimer(uint32_t nTimerID, ITimerHandler* pHandler)
{
	if (nullptr == pHandler)
		return false;

	void** ppTimerInfo = pHandler->GetTimerInfo();
	TIMER_INFO* pTimerInfo = *(TIMER_INFO**)ppTimerInfo;
	if (nullptr == pTimerInfo)
		return false;

	// ɾ�����еĶ�ʱ��
	for (auto itr = pTimerInfo->begin(); itr != pTimerInfo->end(); ++itr)
	{
		if (itr->nTimerID == nTimerID)
		{
			// ��ʱ������ɾ�����timer
			// ��Ϊ�����ڱ����Ĺ����е���KillTimer
			// Ϊ�˱���������������ﲻֱ��ɾ��timer,�������ó�nullptr,������ɾ
			*(itr->itrPos) = nullptr;
			--m_nTimerCount;

			pTimerInfo->erase(itr);
			break;
		}
	}

	if (pTimerInfo->empty())
	{
		delete pTimerInfo;
		pTimerInfo = nullptr;
	}

	return true;
}

void CTimerAxist::CTimerAxist::KillTimer(ITimerHandler* pHandler)
{
	if (nullptr == pHandler)
		return;

	void** ppTimerInfo = pHandler->GetTimerInfo();
	TIMER_INFO* pTimerInfo = *(TIMER_INFO**)ppTimerInfo;
	if (nullptr == pTimerInfo)
		return;

	// ɾ�����еĶ�ʱ��
	for (auto& oneTimer : *pTimerInfo)
	{
		// ��ʱ������ɾ�����timer
		// ��Ϊ�����ڱ����Ĺ����е���KillTimer
		// Ϊ�˱���������������ﲻֱ��ɾ��timer,�������ó�nullptr,������ɾ
		*(oneTimer.itrPos) = nullptr;
		--m_nTimerCount;
	}

	pTimerInfo->clear();
	delete pTimerInfo;
	pTimerInfo = nullptr;
}

void CTimerAxist::CheckTimer()
{
	uint64_t nTimeNow = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
	if (nTimeNow - m_nLastCheckTick < CHECK_FREQUENCY)
		return;

	size_t nSize = m_vecTimerAxis.size();
	if (0 == nSize)
		return;

	uint32_t nLastGrid = uint32_t(((m_nLastCheckTick - m_InitTime) / TIME_GRID_INTERVAL) % nSize);
	uint32_t nCurGrid = uint32_t(((nTimeNow - m_InitTime) / TIME_GRID_INTERVAL) % nSize);
	m_nLastCheckTick = nTimeNow;

	// ����ʱ��̶�
	do
	{
		auto& tCurTimers = m_vecTimerAxis[nLastGrid];

		// ������ǰʱ��̶��е����д�������ʱ��
		for (auto itr = tCurTimers.begin(); itr != tCurTimers.end();)
		{
			Timer* pTimer = *itr;
			if (nullptr == pTimer)
			{
				itr = tCurTimers.erase(itr);
				continue;
			}

			// ������ʱ��
			if (nTimeNow - pTimer->nLastCallTick >= pTimer->nInterval)
			{
				uint64_t nTimeBegin = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
				pTimer->pHandler->OnTimer(pTimer->nTimerID);
				
				// ���һ�����Timer�Ƿ�ɾ����
				if (*itr == pTimer)
				{
					uint64_t nTimeEnd = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
					uint64_t nCostTime = nTimeEnd - nTimeBegin;
					if (nCostTime > TIMER_WARNING_INTERVAL)
					{
						SaveAssertLog("[TimerAxist]��ʱ����΢���棬��ʱ��ID��%d����ʱ�������%d������ʱ�䣺%d��������Ϣ��%s",
							pTimer->nTimerID, pTimer->nInterval, nCostTime, pTimer->strDebugInfo.c_str());

						if (nCostTime > SERIOUS_TIMER_WARNING_INTERVAL)
						{
							SaveAssertLog("[TimerAxist]��ʱ�����ؾ��棬��ʱ��ID��%d����ʱ�������%d������ʱ�䣺%d��������Ϣ��%s",
								pTimer->nTimerID, pTimer->nInterval, nCostTime, pTimer->strDebugInfo.c_str());
						}
					}

					pTimer->nLastCallTick = nTimeNow;
					--pTimer->nCallTimes;

					// ���ô����㹻�ˣ�ɾ��Timer
					if (0 == pTimer->nCallTimes)
					{
						KillTimer(pTimer->nTimerID, pTimer->pHandler);
					}
					else
					{
						// ��Ǩ���´δ����ĵط�
						pTimer->nGridIndex = uint32_t(((pTimer->nLastCallTick - m_InitTime + pTimer->nInterval) / TIME_GRID_INTERVAL) % m_vecTimerAxis.size());
						itr = tCurTimers.erase(itr);
						m_vecTimerAxis[pTimer->nGridIndex].push_back(pTimer);
						pTimer->itrPos = --m_vecTimerAxis[pTimer->nGridIndex].end();
						continue;
					}
				}
				else
				{
					itr = tCurTimers.erase(itr);
					continue;
				}
			}

			++itr;
		}

		if (nLastGrid == nCurGrid)
			break;

		// ������һ��ʱ��̶�
		nLastGrid = (nLastGrid + 1) % m_vecTimerAxis.size();

	} while (nLastGrid != nCurGrid);
}