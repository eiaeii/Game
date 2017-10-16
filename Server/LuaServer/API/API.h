#ifndef _API_WEBSERVER_H
#define _API_WEBSERVER_H
#include "CommonDefine.h"
#include "ExportStruct.h"

// Export Begin

SERVER_API void MyAPITest();

SERVER_API void SendMsgToGBServer(struct Msg_Goods_WEB2GB msg);

// Export End

#endif // _API_WEBSERVER_H