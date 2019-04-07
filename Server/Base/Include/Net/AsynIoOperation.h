#ifndef ASYNIOOPERATION_H
#define ASYNIOOPERATION_H

#include "AsynIoFrame.h"

struct AsynIoHandler;
struct AsynIoBuffer;
struct AsynIoResult;

struct AsynIoOperation_Reader
{
	virtual ~AsynIoOperation_Reader() {};
	
	virtual bool Read(unsigned int nLen, AsynIoHandler *pHandler) = 0;

	virtual bool ReadEx(AsynIoResult *pResult, AsynIoHandler *pHandler) = 0;
};

struct AsynIoOperation_Writer
{
	virtual ~AsynIoOperation_Writer() {}
	
	virtual bool Write(const unsigned char* pData, unsigned int nLen, AsynIoHandler *pHandler) = 0;

	virtual bool WriteEx(AsynIoBuffer *pBuffList, unsigned int nCount, AsynIoHandler *pHandler) = 0;
};

struct AsynIoOperation_Acceptor
{
	virtual ~AsynIoOperation_Acceptor() {}
	
	virtual bool Accept(AsynIoHandler *pHandler) = 0;
};

struct AsynIoOperation_Connector
{
	virtual ~AsynIoOperation_Connector() {}
	
	virtual bool Connect(sockaddr_in * pAddr, AsynIoHandler *pHandler) = 0;
};

#endif // ASYNIOOPERATION_H
