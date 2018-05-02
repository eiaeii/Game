#ifndef _Log_HPP
#define _Log_HPP

#include "API.h"
#include "TimeManager.h"

#include <vector>
#include <string>
#include <atomic>
#include <string.h>
#include <thread>
#include <sstream>
#include <experimental/filesystem>

#ifdef _WINDOWS
	#include <Windows.h>
#endif // !_WINDOWS

#define MAX_LOG_CACHE_SIZE  1024 * 1024 * 4

class CLog
{
public:
	enum
	{
		Log_Debug,
		Log_Assert,
		Log_Num,
	};

public:
	~CLog();

	bool Init();

	void SaveLog(unsigned char btLogType, const char *pszMsg);

	void SaveLogEx(unsigned char btLogType, const char * pszFile, const char * pszFunction, unsigned long unLine, std::thread::id threadID, const char * pszLog, ...);
	
private:
	void FlushAllLogToFile();

	void SaveLogToCache(unsigned char btLogType, const char *pszBuffer, size_t nLength);
	
	void FlushLogToFile(unsigned char btLogType);
	
private:
	std::vector<std::string> m_vecLogFile;
	std::string m_strProcessName;
	std::atomic_uchar m_LogSaveFlag[Log_Num] = { 0 };
	std::atomic_uchar m_LogPrintFlag[Log_Num] = { 0 };
	std::atomic_char* m_LogCache[Log_Num] = { nullptr };
	size_t m_LogPos[Log_Num] = { 0 };

#ifdef _WINDOWS
	unsigned short m_windowColor[Log_Num] =
	{
		FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN,
		FOREGROUND_RED,
	};
#endif // !_WINDOWS
};

extern CLog *g_pLogSystem;
SERVER_API CLog* CreateLogSystem();
SERVER_API CLog* GetLogSystem();

//传入的msgLog必须是字符串
#define SaveDebugLog(msgLog, ...)	(GetLogSystem()->SaveLogEx(CLog::Log_Debug, __FILE__, __FUNCTION__, __LINE__, std::this_thread::get_id(), msgLog, ##__VA_ARGS__))
#define SaveAssertLog(msgLog, ...)	(GetLogSystem()->SaveLogEx(CLog::Log_Assert, __FILE__, __FUNCTION__, __LINE__, std::this_thread::get_id(), msgLog, ##__VA_ARGS__))

#endif // _Log_HPP

