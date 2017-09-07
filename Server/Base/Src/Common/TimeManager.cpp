#include "TimeManager.h"

unsigned long CTimeManager::GetYYYYMMDD()
{
	struct tm objNew = { 0 };
	std::time_t tCurrentTime = time(nullptr);
	localtime_s(&objNew, &tCurrentTime);
	return ((objNew.tm_year + 1900) * 10000 + (objNew.tm_mon + 1) * 100 + objNew.tm_mday);
}

std::string CTimeManager::GetYYYYMMDDString()
{
	char szTime[32] = { 0 };
	struct tm objNew = { 0 };
	std::time_t tCurTime = time(nullptr);
	localtime_s(&objNew, &tCurTime);
	sprintf_s(szTime, sizeof(szTime), "%04d-%02d-%02d", (objNew.tm_year + 1900), (objNew.tm_mon + 1), objNew.tm_mday);
	return szTime;
}

std::string CTimeManager::GetYYYYMMDDHHMMSSString()
{
	char szTime[64];
	std::time_t tCurrentTime = time(NULL);
	struct tm objNew = { 0 };
	localtime_s(&objNew, &tCurrentTime);
	sprintf_s(szTime, sizeof(szTime), "%04d-%02d-%02d %02d:%02d:%02d", (objNew.tm_year + 1900), objNew.tm_mon + 1, objNew.tm_mday, objNew.tm_hour, objNew.tm_min, objNew.tm_sec);
	return szTime;
}


