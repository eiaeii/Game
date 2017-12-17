#ifndef _WEBSERVER_API_H
#define _WEBSERVER_API_H
#include "CommonDefine.h"
#include "ExportStruct.h"

// Export Begin

SERVER_API TestStruct API_TestStruct(TestStruct stIn);
SERVER_API TestStruct* API_TestArray(TestStruct *stIn, int n);

// Export End

#endif // _WEBSERVER_API_H