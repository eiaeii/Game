#include "Log.h"
#include <stdarg.h>

CLog *g_pLogSystem = nullptr;

CLog::~CLog()
{
	FlushAllLogToFile();

	for (short i = 0; i < Log_Num; ++i)
	{
		delete[] m_LogCache[i];
		m_LogCache[i] = nullptr;
	}
};


bool CLog::Init()
{
	std::experimental::filesystem::create_directory("Log");

	auto curPath = std::experimental::filesystem::current_path();
	m_strProcessName = curPath.stem().string();

	m_vecLogFile.resize(Log_Num);
	m_vecLogFile[Log_Debug] = "./Log/[Debug]";
	m_vecLogFile[Log_Assert] = "./Log/[Assert]";
	
	for (unsigned char i = 0; i < Log_Num; ++i)
	{
		m_LogSaveFlag[i] = 1;
		m_LogPrintFlag[i] = 1;

		if (nullptr == m_LogCache[i])
			m_LogCache[i] = new std::atomic_char[MAX_LOG_CACHE_SIZE];
		if (nullptr == m_LogCache[i])
		{
			printf("[Error] Log Cache New Buffer Error, LogType:%hhd, Function:%s, Line:%d\n", i, __FUNCTION__, __LINE__);
			return false;
		}
		m_LogPos[i] = 0;
	}

#ifndef _DEBUG
	m_LogSaveFlag[Log_Debug] = 0;
	m_LogPrintFlag[Log_Debug] = 0;
#endif // !_DEBUG

	return true;
}

void CLog::SaveLog(unsigned char btLogType, const char *pszMsg)
{
	if (0 == m_LogSaveFlag[btLogType] && 0 == m_LogPrintFlag[btLogType])
		return;

	std::stringstream ss;

	ss << "【Lua】" << pszMsg << "\n\n";

	auto strLog = ss.str();
	
	auto colorFunc = [&](unsigned char btType)
	{
		if (m_LogPrintFlag[btType])
		{
#ifdef _WINDOWS
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN);
#endif // !_WINDOWS
			printf(strLog.c_str());
		}

	};

	colorFunc(btLogType);

	if (1 == m_LogSaveFlag[btLogType])
		SaveLogToCache(btLogType, strLog.c_str(), strLog.length());
}

void CLog::SaveLogEx(unsigned char btLogType, const char * pszFile, const char * pszFunction, unsigned long unLine, std::thread::id threadID, const char * pszLog, ...)
{
	if (0 == m_LogSaveFlag[btLogType] && 0 == m_LogPrintFlag[btLogType])
		return;

	char szTemLogFormat[4096] = { 0 };
	va_list argptr;
	va_start(argptr, pszLog);
	vsnprintf(szTemLogFormat, sizeof(szTemLogFormat), pszLog, argptr);
	va_end(argptr);

	std::stringstream ss;

	ss << szTemLogFormat << "\n";
	ss << "日志信息：\n";
	ss << "\t" << pszFile << ":" << unLine << ":" << "in function '" << pszFunction << "'\n";
	ss << "\tThread:" << threadID << "  Time:" << CTimeManager::Instance()->GetYYYYMMDDHHMMSSString() << "\n\n";
	
	auto strLog = ss.str();

	auto colorFunc = [&](unsigned char btType)
	{
		if (m_LogPrintFlag[btType])
		{
#ifdef _WINDOWS
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), m_windowColor[btType]);
#endif // !_WINDOWS
			printf(strLog.c_str());
#ifdef _WINDOWS
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN);
#endif // !_WINDOWS
		}

	};

	colorFunc(btLogType);

	if (1 == m_LogSaveFlag[btLogType])
		SaveLogToCache(btLogType, strLog.c_str(), strLog.length());
}

void CLog::FlushAllLogToFile()
{
	for (unsigned char i = 0; i < Log_Num; ++i)
		FlushLogToFile(i);
}

void CLog::SaveLogToCache(unsigned char btLogType, const char *pszBuffer, size_t nLength)
{
	if (nullptr == pszBuffer)
	{
		printf("[Error] SaveLogToCache, pszBuffer is null, Function:%s, Line:%d\n", __FUNCTION__, __LINE__);
		return;
	}

	if (btLogType > Log_Num)
		return;

	auto &nCurPos = m_LogPos[btLogType];
	if (MAX_LOG_CACHE_SIZE - nCurPos < nLength)
	{
		this->FlushLogToFile(btLogType);
	}

	memcpy(&m_LogCache[btLogType][nCurPos], pszBuffer, nLength);
	nCurPos += nLength;

	if (Log_Assert == btLogType
		|| Log_Debug == btLogType
		)
		FlushLogToFile(btLogType);
}

void CLog::FlushLogToFile(unsigned char btLogType)
{
	if (btLogType > Log_Num)
		return;

	std::string strFile = "";
	if (Log_Num != btLogType)
	{
		if (0 == m_LogSaveFlag[btLogType])
			return;

		if (nullptr == m_LogCache[btLogType] || 0 == m_LogPos)
			return;

		strFile = m_vecLogFile[btLogType] + m_strProcessName + "_" + CTimeManager::Instance()->GetYYYYMMDDString() + ".log";
		FILE *pFile = nullptr;
#ifdef _WINDOWS
		fopen_s(&pFile, strFile.c_str(), "a+");
#else
		pFile = fopen(strFile.c_str(), "a+");
#endif // _WINDOWS
		if (nullptr == pFile)
		{
			printf("[Error] Open LogFile Fail! FileName:%s, Function:%s, Line:%d\n", strFile.c_str(), __FUNCTION__, __LINE__);
			m_LogPos[btLogType] = 0;
			return;
		}

		fwrite(m_LogCache[btLogType], 1, m_LogPos[btLogType], pFile);
		m_LogPos[btLogType] = 0;
		fclose(pFile);
		return;
	}
	else
	{
		for (size_t i = 0; i < Log_Num; ++i)
		{
			if (0 == m_LogSaveFlag[i])
				continue;

			if (nullptr == m_LogCache[i] || 0 == m_LogPos)
				continue;

			strFile = m_vecLogFile[i] + m_strProcessName + "_" + CTimeManager::Instance()->GetYYYYMMDDString() + ".log";
			FILE *pFile = nullptr;
#ifdef _WINDOWS
			fopen_s(&pFile, strFile.c_str(), "a+");
#else
			pFile = fopen(strFile.c_str(), "a+");
#endif // _WINDOWS
			if (nullptr == pFile)
			{
				printf("[Error] Open LogFile Fail! FileName:%s, Function:%s, Line:%d\n", strFile.c_str(), __FUNCTION__, __LINE__);
				continue;
			}

			fwrite(m_LogCache[i], 1, m_LogPos[i], pFile);
			m_LogPos[i] = 0;
			fclose(pFile);
		}
	}
}

CLog* CreateLogSystem()
{
	if (nullptr != g_pLogSystem)
	{
		return g_pLogSystem;
	}

	g_pLogSystem = new CLog();
	if (nullptr == g_pLogSystem)
	{

		printf("日志系统创建失败！");
		return nullptr;
	}

	g_pLogSystem->Init();

	return g_pLogSystem;
}

CLog* GetLogSystem()
{
	if (nullptr == g_pLogSystem)
	{
		return CreateLogSystem();
	}

	return g_pLogSystem;
}