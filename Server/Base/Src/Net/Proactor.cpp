/***********************************************************************
** �ļ�����Proactor.cpp
** ��  �ߣ���ˮ��(765865368@qq.com)
** ʱ  �䣺2018-12-15
** ��  �����첽�¼��ֽ�ģ�ͣ�ǰ������
************************************************************************/

#include "Proactor.h"

bool Proactor::Create(unsigned long nThreadNum)
{
	return false;
}

void Proactor::Close()
{
}

bool Proactor::ResgisterDevice(AsynIoDevice * pDevice)
{
	return false;
}

bool Proactor::HandleEvents(unsigned long nTimeOut)
{
	return false;
}

void Proactor::HandleEventsLoop()
{
}

bool Proactor::PostCompletion(AsynIoResult * pResult)
{
	return false;
}
