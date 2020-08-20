#include "TimerAxis.hpp"
#include <chrono>

ITimerHandler::~ITimerHandler()
{

}

CTimerAxist::CTimerAxist()
{
	// 将时间轴进行分割
	m_vecTimerAxis.resize((TIME_AXIS_LENGTH + TIME_GRID - 1) / TIME_GRID);
	m_InitTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
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
	nTimerID; ; nInterval;  pHandler; nCallTimes; pszDebugInfo;
	return false;
}

bool CTimerAxist::CTimerAxist::KillTimer(uint32_t nTimerID, ITimerHandler* pHandler)
{
	nTimerID; pHandler;
	return false;
}

void CTimerAxist::CTimerAxist::KillTimer(ITimerHandler* pHandler)
{
	pHandler;
}