#include "Log.h"

extern "C"
{
#include "WebServerAPI.h"
}

TestStruct TestAPI(TestStruct stIn)
{
	TestStruct st;
	st.a = stIn.a;
	st.b = stIn.b;
	return st;
}