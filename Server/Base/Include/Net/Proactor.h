/***********************************************************************
** 文件名：Proactor.h
** 作  者：何水大(765865368@qq.com)
** 时  间：2018-12-15
** 描  述：异步事件分解模型（前摄器）
************************************************************************/
#ifndef _PROACTOR_H
#define _PROACTOR_H

struct AsynIoDevice;
struct AsynIoResult;

struct Proactor
{
	/**
	*作用			：创建完成端口
	*参数 nThreadNum：完成端口的工作线程数量
	*提示			：如果nThreadNum为0，则启用系统CPU个数相等的工作线程
					  推荐数量为：CPU个数 * 2 + 2
	*/
	bool Create(unsigned long nThreadNum = 0) override;

	/**
	*作用			：关闭完成端口
	*/
	void Close() override;

	bool ResgisterDevice(AsynIoDevice *pDevice) override;

	bool HandleEvents(unsigned long nTimeOut = INFINITE) override;

	void HandleEventsLoop() override;

	bool PostCompletion(AsynIoResult *pResult) override;
};

#endif // _PROACTOR_H