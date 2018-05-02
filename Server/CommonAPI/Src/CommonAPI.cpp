#include "Log.h"
extern "C"
{
#include "CommonAPI.h"
}

void SaveDebugLog_Lua(const char *pszLog)
{
	GetLogSystem()->SaveLog(CLog::Log_Debug, pszLog);
}

void SaveAssertLog_Lua(const char *pszLog)
{
	GetLogSystem()->SaveLog(CLog::Log_Assert, pszLog);
}

struct TestStruct API_TestStruct(struct TestStruct stIn, struct TestStruct stOut)
{
	TestStruct st;
	st.a = stIn.a;
	st.b = stIn.b;
	stOut = st;
	return st;
}

struct TestStruct API_TestArray(struct TestStruct *stIn, int nIn, struct TestStruct *stOut, int *nOut)
{
	struct TestStruct st;

	for (int i = 0; i < nIn; ++i)
	{
		st.a += stIn[i].a;
		st.b += stIn[i].b;
		stOut[i].a = st.a;
		stOut[i].b = st.a;
	}

	*nOut = 3;

	return st;
}

int API_TestPointer(struct TestStruct *stIn, int nIn)
{
	return stIn->a + stIn->b + nIn;
}
