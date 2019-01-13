/***********************************************************************
** �ļ�����Proactor.h
** ��  �ߣ���ˮ��(765865368@qq.com)
** ʱ  �䣺2018-12-15
** ��  �����첽�¼��ֽ�ģ�ͣ�ǰ������
************************************************************************/
#ifndef _PROACTOR_H
#define _PROACTOR_H

struct AsynIoDevice;
struct AsynIoResult;

struct Proactor
{
	/**
	*����			��������ɶ˿�
	*���� nThreadNum����ɶ˿ڵĹ����߳�����
	*��ʾ			�����nThreadNumΪ0��������ϵͳCPU������ȵĹ����߳�
					  �Ƽ�����Ϊ��CPU���� * 2 + 2
	*/
	bool Create(unsigned long nThreadNum = 0) override;

	/**
	*����			���ر���ɶ˿�
	*/
	void Close() override;

	bool ResgisterDevice(AsynIoDevice *pDevice) override;

	bool HandleEvents(unsigned long nTimeOut = INFINITE) override;

	void HandleEventsLoop() override;

	bool PostCompletion(AsynIoResult *pResult) override;
};

#endif // _PROACTOR_H