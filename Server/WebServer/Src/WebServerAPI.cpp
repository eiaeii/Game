#include "Log.h"
extern "C"
{
#include "WebServerAPI.h"
}

TestStruct API_TestStruct(TestStruct stIn, TestStruct stOut)
{
	TestStruct st;
	st.a = stIn.a;
	st.b = stIn.b;
	stOut = st;
	return st;
}

TestStruct API_TestArray(TestStruct *stIn, int nIn, TestStruct *stOut, int *nOut)
{
	TestStruct st;

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

int API_TestPointer(TestStruct *stIn, int nIn)
{
	return stIn->a + stIn->b + nIn;
}