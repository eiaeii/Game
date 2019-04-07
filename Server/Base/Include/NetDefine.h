#ifndef _NETDEFINE_H
#define _NETDEFINE_H

// 最大Buff数目
constexpr int MAX_ASYN_IO_BUFF_COUNT = 128;

enum  class AsynIoType : unsigned char
{
	AsynIoType_Unknow = 0,
	AsynIoType_Read,
	AsynIoType_Write,
	AsynIoType_Accept,
	AsynIoType_Connect,
	AsynIoType_ReadFrom,
	AsynIoType_WriteTo,
	AsynIoType_Close,
	AsynIoType_DisConnect,
};

#endif // _NETDEFINE_H

