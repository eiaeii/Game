#include <vector>
#include "Singleton.hpp"
#include <Windows.h>
#include <string>
#include <atomic>
#include "CommonDefine.h"
#include "TimeManager.h"
#include <string.h>
#include <thread>
#include <sstream>
#include <filesystem>

#ifndef _Log_HPP
#define _Log_HPP

#define MAX_LOG_CACHE_SIZE  1024 * 1024 * 4

class CLog : public CSingleton<CLog>
{
public:
	enum
	{
		Log_Debug,
		Log_Assert,
		Log_Player,
		Log_Mail,
		Log_Recharge,
		Log_Num,
	};

public:
	~CLog();

	bool Init();

	void SaveLogEx(unsigned char btLogType, const char * pszFunction, unsigned long unLine, std::thread::id threadID, const char * pszLog, ...);
	
	void FlushAllLogToFile();
	
private:
	void SaveLogToCache(unsigned char btLogType, char* pszBuffer, size_t nLength);
	
	void FlushLogToFile(unsigned char btLogType);
	
private:
	std::vector<std::string> m_vecLogFile;
	std::string m_strProcessName;
	unsigned char m_LogSaveFlag[Log_Num] = { 0 };
	unsigned char m_LogPrintFlag[Log_Num] = { 0 };
	std::atomic_char* m_LogCache[Log_Num] = { nullptr };
	size_t m_LogPos[Log_Num] = { 0 };
};

//传入的msgLog必须是字符串
#define SaveDebugLog(msgLog, ...)	(CLog::Instance()->SaveLogEx(CLog::Log_Debug, __FUNCTION__, __LINE__, std::this_thread::get_id(), msgLog, ##__VA_ARGS__))
#define SaveAssertLog(msgLog, ...)	(CLog::Instance()->SaveLogEx(CLog::Log_Assert, __FUNCTION__, __LINE__, std::this_thread::get_id(), msgLog, ##__VA_ARGS__))
#define SavePlayerLog(msgLog, ...)	(CLog::Instance()->SaveLogEx(CLog::Log_Player, __FUNCTION__, __LINE__, std::this_thread::get_id(), msgLog, ##__VA_ARGS__))
#define SaveMailLog(msgLog, ...)	(CLog::Instance()->SaveLogEx(CLog::Log_Mail, __FUNCTION__, __LINE__, std::this_thread::get_id(), msgLog, ##__VA_ARGS__))
#define SaveRechargeLog(msgLog, ...)	(CLog::Instance()->SaveLogEx(CLog::Log_Recharge, __FUNCTION__, __LINE__, std::this_thread::get_id(), msgLog, ##__VA_ARGS__))

#endif // _Log_HPP

