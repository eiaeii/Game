#include "Log.h"

CLog::~CLog()
{
	for (short i = 0; i < Log_Num; ++i)
		SAFE_DELETE_ARRAY(m_LogCache[i]);
};


bool CLog::Init(const char* pszFileName)
{
	if (pszFileName != nullptr && strlen(pszFileName) > 1)
	m_strProcessName = pszFileName;
	else
	{
		char strProcessPath[256] = { 0 };

#ifdef _LINUX_
		if (readlink("/proc/self/exe", strProcessPath, sizeof(strProcessPath)) <= 0)
		{
			return false;
		}
		char *strProcessName = strrchr(strProcessPath, '/');
		++strProcessName;
		strchr(strProcessName, '.')[0] = 0;
		m_strProcessName = strProcessName;

#endif // _LINUX_

#ifdef _WINDOWS_
		char *pPath = nullptr;
		_get_pgmptr(&pPath);
		char *strProcessName = strrchr(pPath, '\\');
		++strProcessName;
		strchr(strProcessName, '.')[0] = 0;
		m_strProcessName = strProcessName;
#endif // _WINDOWS_
	}

	m_vecLogFile.resize(Log_Num);
	m_vecLogFile[Log_Debug] = "./Log/[Debug]";
	m_vecLogFile[Log_Assert] = "./Log/[Assert]";
	m_vecLogFile[Log_Player] = "./Log/[Player]";
	m_vecLogFile[Log_Mail] = "./Log/[Mail]";
	m_vecLogFile[Log_Recharge] = "./Log/[Recharge]";

	for (size_t i = 0; i < Log_Num; ++i)
	{
		m_LogSaveFlag[i] = 1;
	}

#ifndef _DEBUG
	m_LogSaveFlag[Log_Debug] = 0;
#endif // !_DEBUG


	return true;
}

void CLog::SaveLogEx(unsigned char btLogType, const char * pszFunction, unsigned long unLine, std::thread::id threadID, const char * pszLog, ...)
{
	char szTemLogFormat[4096] = { 0 };
	va_list argptr;
	va_start(argptr, pszLog);
	vsnprintf_s(szTemLogFormat, sizeof(szTemLogFormat), pszLog, argptr);
	va_end(argptr);

	std::stringstream ss;

	ss << " Thread [ ";
	ss << threadID;
	ss << " ]";

	ss << " Func [ ";
	ss << pszFunction;
	ss << " ]";

	ss << " Line [ ";
	ss << unLine;
	ss << " ]";

	ss << " Time [ ";
	ss << CTimeManager::Instance()->GetYYYYMMDDHHMMSSString();
	ss << " ]";

	ss << "\n";

	strcat_s(szTemLogFormat, ss.str().c_str());

	SaveLogToCache(btLogType, szTemLogFormat, strlen(szTemLogFormat));

	printf_s(szTemLogFormat);
}

void CLog::FlushAllLogToFile()
{
	for (unsigned char i = 0; i < Log_Num; ++i)
		FlushLogToFile(i);
}

void CLog::SaveLogToCache(unsigned char btLogType, char* pszBuffer, size_t nLength)
{
	if (nullptr == pszBuffer)
	{
		printf_s("[Error] SaveLogToCache, pszBuffer is null, Function:%s, Line:%d\n", __FUNCTION__, __LINE__);
		return;
	}

	if (btLogType > Log_Num)
		return;

	if (1 == m_LogSaveFlag[btLogType])
	{
		if (nullptr == m_LogCache[btLogType])
			m_LogCache[btLogType] = new std::atomic_char[MAX_LOG_CACHE_SIZE];
		if (nullptr == m_LogCache[btLogType])
			printf("[Error] Log Cache New Buffer Error, LogType:%hhd, Function:%s, Line:%d\n", btLogType, __FUNCTION__, __LINE__);
		m_LogPos[btLogType] = 0;
	}

	if (MAX_LOG_CACHE_SIZE - m_LogPos[btLogType] < nLength)
	{
		this->FlushLogToFile(btLogType);
	}

	memcpy(&m_LogCache[btLogType][m_LogPos[btLogType]], pszBuffer, nLength);
	m_LogPos[btLogType] += nLength;

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
		fopen_s(&pFile, strFile.c_str(), "a+");
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
			fopen_s(&pFile, strFile.c_str(), "a+");
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