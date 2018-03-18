#ifndef _WEBSERVER_API_H
#define _WEBSERVER_API_H
#include "CommonDefine.h"
#include "ExportStruct.h"

// Export Begin

SERVER_API TestStruct API_TestStruct(TestStruct stIn, TestStruct stOut);
SERVER_API TestStruct API_TestArray(TestStruct *stIn, int nIn, TestStruct *stOut, int *nOut);
SERVER_API int API_TestPointer(TestStruct *stIn, int nIn);

// Export End

#endif // _WEBSERVER_API_H