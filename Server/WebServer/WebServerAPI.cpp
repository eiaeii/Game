#include "Log.h"

extern "C"
{
#include "WebServerAPI.h"
}

TestStruct API_TestStruct(TestStruct stIn)
{
	TestStruct st;
	st.a = stIn.a;
	st.b = stIn.b;
	return st;
}

SERVER_API TestStruct* API_TestArray(TestStruct *stIn, int n)
{
	if (!stIn)
		return nullptr;

	for (int i = 0; i < n; ++i)
	{

	}

	return nullptr;
}