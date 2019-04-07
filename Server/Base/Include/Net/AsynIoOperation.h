#ifndef ASYNIOOPERATION_H
#define ASYNIOOPERATION_H

#include "AsynIoFrame.h"

struct IAsynIoHandler;
struct IAsynIoBuffer;
struct IAsynIoResult;

struct AsynIoOperation_Reader
{
	virtual ~AsynIoOperation_Reader() {};
	
	virtual bool Read(unsigned int nLen, IAsynIoHandler *pHandler) = 0;

	virtual bool ReadEx(IAsynIoResult *pResult, IAsynIoHandler *pHandler) = 0;
};

struct AsynIoOperation_Writer
{
	virtual ~AsynIoOperation_Writer() {}
	
	virtual bool Write(const unsigned char* pData, unsigned int nLen, IAsynIoHandler *pHandler) = 0;

	virtual bool WriteEx(AsynIoBuffer *pBuffList, unsigned int nCount, IAsynIoHandler *pHandler) = 0;
};

struct AsynIoOperation_Acceptor
{
	virtual ~AsynIoOperation_Acceptor() {}
	
	virtual bool Accept(IAsynIoHandler *pHandler) = 0;
};

struct AsynIoOperation_Connector
{
	virtual ~AsynIoOperation_Connector() {}
	
	virtual bool Connect(sockaddr_in * pAddr, IAsynIoHandler *pHandler) = 0;
};

#endif // ASYNIOOPERATION_H
