/***********************************************************************
** 文件名：IProactor.h
** 作  者：何水大(765865368@qq.com)
** 时  间：2018-12-15
** 描  述：异步事件分解模型（前摄器）
************************************************************************/
#ifndef _IPROACTOR_H
#define _IPROACTOR_H

#include "CommonDefine.h"

struct AsynIoDevice;
struct AsynIoResult;

struct IProactor
{
	/**
	*作用			：创建完成端口
	*参数 nThreadNum：完成端口的工作线程数量
	*提示			：如果nThreadNum为0，则启用系统CPU个数相等的工作线程
					  推荐数量为：CPU个数 * 2 + 2
	*/
	virtual bool Create(unsigned long nThreadNum = 0) = 0;

	/**
	*作用			：关闭完成端口
	*/
	virtual void Close() = 0;

	virtual bool ResgisterDevice(AsynIoDevice *pDevice) = 0;

	virtual bool HandleEvents(unsigned long nTimeOut = INFINITE) = 0;

	virtual void HandleEventsLoop() = 0;

	virtual bool PostCompletion(AsynIoResult *pResult) = 0;
};

#endif // _IPROACTOR_H