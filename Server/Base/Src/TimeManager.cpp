#include "TimeManager.h"

unsigned long CTimeManager::GetYYYYMMDD()
{
	std::time_t tCurrentTime = time(nullptr);
#ifdef _WINDOWS
	struct tm objNew;
	localtime_s(&objNew, &tCurrentTime);
	return ((objNew.tm_year + 1900) * 10000 + (objNew.tm_mon + 1) * 100 + objNew.tm_mday);
#else
	auto objNew = localtime(&tCurrentTime);
	return ((objNew->tm_year + 1900) * 10000 + (objNew->tm_mon + 1) * 100 + objNew->tm_mday);
#endif // _WINDOWS
}

std::string CTimeManager::GetYYYYMMDDString()
{
	char szTime[64];
	std::time_t tCurrentTime = time(nullptr);
#ifdef _WINDOWS
	struct tm objNew;
	localtime_s(&objNew, &tCurrentTime);
	sprintf_s(szTime, "%04d-%02d-%02d", (objNew.tm_year + 1900), (objNew.tm_mon + 1), objNew.tm_mday);
#else
	auto objNew = localtime(&tCurrentTime);
	sprintf(szTime, "%04d-%02d-%02d", (objNew->tm_year + 1900), (objNew->tm_mon + 1), objNew->tm_mday);
#endif // _WINDOWS

	return szTime;
}

std::string CTimeManager::GetYYYYMMDDHHMMSSString()
{
	char szTime[64];
	std::time_t tCurrentTime = time(nullptr);
#ifdef _WINDOWS
	struct tm objNew;
	localtime_s(&objNew, &tCurrentTime);
	sprintf_s(szTime, "%04d-%02d-%02d %02d:%02d:%02d", (objNew.tm_year + 1900), objNew.tm_mon + 1, objNew.tm_mday, objNew.tm_hour, objNew.tm_min, objNew.tm_sec);
#else
	auto objNew = localtime(&tCurrentTime);
	sprintf(szTime, "%04d-%02d-%02d %02d:%02d:%02d", (objNew->tm_year + 1900), objNew->tm_mon + 1, objNew->tm_mday, objNew->tm_hour, objNew->tm_min, objNew->tm_sec);
#endif // _WINDOWS

	return szTime;
}


