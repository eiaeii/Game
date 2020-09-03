#include "TimerAxis.hpp"
#include <chrono>
#include "Log.h"

ITimerHandler::~ITimerHandler()
{
	CTimerAxist::GetInstance()->KillTimer(static_cast<ITimerHandler*>(this));
}

CTimerAxist::CTimerAxist()
{
	// 将时间轴划分为N个刻度
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

	// 检查是否添加了这个timer
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

	// 将Timer添加到对应的时间刻度
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

	// 删除其中的定时器
	for (auto itr = pTimerInfo->begin(); itr != pTimerInfo->end(); ++itr)
	{
		if (itr->nTimerID == nTimerID)
		{
			// 从时间轴中删除这个timer
			// 因为可能在遍历的过程中调用KillTimer
			// 为了避免遍历崩溃，这里不直接删除timer,而是先置成nullptr,后面再删
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

	// 删除其中的定时器
	for (auto& oneTimer : *pTimerInfo)
	{
		// 从时间轴中删除这个timer
		// 因为可能在遍历的过程中调用KillTimer
		// 为了避免遍历崩溃，这里不直接删除timer,而是先置成nullptr,后面再删
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

	// 遍历时间刻度
	do
	{
		auto& tCurTimers = m_vecTimerAxis[nLastGrid];

		// 遍历当前时间刻度中的所有待触发定时器
		for (auto itr = tCurTimers.begin(); itr != tCurTimers.end();)
		{
			Timer* pTimer = *itr;
			if (nullptr == pTimer)
			{
				itr = tCurTimers.erase(itr);
				continue;
			}

			// 触发定时器
			if (nTimeNow - pTimer->nLastCallTick >= pTimer->nInterval)
			{
				uint64_t nTimeBegin = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
				pTimer->pHandler->OnTimer(pTimer->nTimerID);
				
				// 检查一下这个Timer是否被删除了
				if (*itr == pTimer)
				{
					uint64_t nTimeEnd = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
					uint64_t nCostTime = nTimeEnd - nTimeBegin;
					if (nCostTime > TIMER_WARNING_INTERVAL)
					{
						SaveAssertLog("[TimerAxist]定时器轻微警告，定时器ID：%d，定时器间隔：%d，消耗时间：%d，调试信息：%s",
							pTimer->nTimerID, pTimer->nInterval, nCostTime, pTimer->strDebugInfo.c_str());

						if (nCostTime > SERIOUS_TIMER_WARNING_INTERVAL)
						{
							SaveAssertLog("[TimerAxist]定时器严重警告，定时器ID：%d，定时器间隔：%d，消耗时间：%d，调试信息：%s",
								pTimer->nTimerID, pTimer->nInterval, nCostTime, pTimer->strDebugInfo.c_str());
						}
					}

					pTimer->nLastCallTick = nTimeNow;
					--pTimer->nCallTimes;

					// 调用次数足够了，删除Timer
					if (0 == pTimer->nCallTimes)
					{
						KillTimer(pTimer->nTimerID, pTimer->pHandler);
					}
					else
					{
						// 搬迁到下次触发的地方
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

		// 进入下一个时间刻度
		nLastGrid = (nLastGrid + 1) % m_vecTimerAxis.size();

	} while (nLastGrid != nCurGrid);
}