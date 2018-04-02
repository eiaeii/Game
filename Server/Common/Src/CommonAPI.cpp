#include "Log.h"
extern "C"
{
#include "CommonAPI.h"
}

void SaveDebugLog_Lua(const char *pszLog)
{
	CLog::Instance()->SaveLog(CLog::Log_Debug, pszLog);
}

void SaveAssertLog_Lua(const char *pszLog)
{
	CLog::Instance()->SaveLog(CLog::Log_Assert, pszLog);
}
