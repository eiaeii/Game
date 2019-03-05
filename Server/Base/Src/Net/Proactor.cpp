/***********************************************************************
** 文件名：Proactor.cpp
** 作  者：何水大(765865368@qq.com)
** 时  间：2018-12-15
** 描  述：异步事件分解模型（前摄器）
************************************************************************/

#include "Proactor.h"

bool Proactor::Create(unsigned long nThreadNum)
{
	nThreadNum;
	return false;
}

void Proactor::Close()
{
}

bool Proactor::ResgisterDevice(AsynIoDevice * pDevice)
{
	pDevice;
	return false;
}

bool Proactor::HandleEvents(unsigned long nTimeOut)
{
	nTimeOut;
	return false;
}

void Proactor::HandleEventsLoop()
{
}

bool Proactor::PostCompletion(AsynIoResult * pResult)
{
	pResult;
	return false;
}
