#include "API.h"
#include <stdio.h>
#include "Log.h"

void MyAPITest()
{
	SaveAssertLog("API导出成功！");
}

void SendMsgToGBServer(struct Msg_Goods_WEB2GB msg)
{
	msg;
	SaveAssertLog("执行 SendMsgToGBServer ！");
}
