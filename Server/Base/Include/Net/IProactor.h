/***********************************************************************
** �ļ�����IProactor.h
** ��  �ߣ���ˮ��(765865368@qq.com)
** ʱ  �䣺2018-12-15
** ��  �����첽�¼��ֽ�ģ�ͣ�ǰ������
************************************************************************/
#ifndef _IPROACTOR_H
#define _IPROACTOR_H

#include "CommonDefine.h"

struct AsynIoDevice;
struct AsynIoResult;

struct IProactor
{
	/**
	*����			��������ɶ˿�
	*���� nThreadNum����ɶ˿ڵĹ����߳�����
	*��ʾ			�����nThreadNumΪ0��������ϵͳCPU������ȵĹ����߳�
					  �Ƽ�����Ϊ��CPU���� * 2 + 2
	*/
	virtual bool Create(unsigned long nThreadNum = 0) = 0;

	/**
	*����			���ر���ɶ˿�
	*/
	virtual void Close() = 0;

	virtual bool ResgisterDevice(AsynIoDevice *pDevice) = 0;

	virtual bool HandleEvents(unsigned long nTimeOut = INFINITE) = 0;

	virtual void HandleEventsLoop() = 0;

	virtual bool PostCompletion(AsynIoResult *pResult) = 0;
};

#endif // _IPROACTOR_H