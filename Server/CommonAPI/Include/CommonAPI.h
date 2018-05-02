#ifndef _COMMON_API_H
#define _COMMON_API_H
#include "CommonDefine.h"

extern "C"
{
#include "ExportStruct.h"
}
// Export Begin

SERVER_API void SaveDebugLog_Lua(const char *pszLog);

SERVER_API void SaveAssertLog_Lua(const char *pszLog);

SERVER_API struct TestStruct API_TestStruct(struct TestStruct stIn, struct TestStruct stOut);

SERVER_API struct TestStruct API_TestArray(struct TestStruct *stIn, int nIn, struct TestStruct *stOut, int *nOut);

SERVER_API int API_TestPointer(struct TestStruct *stIn, int nIn);

// Export End

#endif // _COMMON_API_H