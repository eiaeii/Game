/***********************************************************************
** 文件名：AsynIoFrame.h
** 作  者：何水大(765865368@qq.com)
** 时  间：2018-11-28
** 描  述：异步IO框架
************************************************************************/
#ifndef ASYNIOFRAME_H
#define ASYNIOFRAME_H

// 最大Buff数目
constexpr int MAX_ASYN_IO_BUFF_COUNT = 128;

struct AsynIoHandler;
struct AsynIoBuffer;
struct AsynIoDevice;

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

/**
*名称：异步IO处理器
*说明：常规用法：
		1、发起一个异步IO操作时，指定一个异步IO处理器
		2、当异步IO操作完成时，调用之前指定的处理器处理IO数据
*/
struct AsynIoHandler
{
	/**
	*作用			：异步IO完成时回调该函数（在这里写处理代码）
	*参数 pResult	：异步IO完成对象，通过它可以获取IO的完成情况，
					  例如是否成功，传输了多少字节以及字节的内容等
	*/
	virtual void HandleIoComplete(AsynIoResult* pResult) = 0;

	/**
	*作用：释放引用
	*提示：由于AsynIoHandler可能被多个线程调用，所以必须支持引用计数机制，
		   否则消耗会有问题，因为你不知道是否有其他的线程正在引用
	*/
	virtual void ReleaseRef() = 0;

	/**
	*作用：增加引用计数
	*提示：由于AsynIoHandler可能被多个线程调用，所以必须支持引用计数机制，
		   否则消耗会有问题，因为你不知道是否有其他的线程正在引用
	*/
	virtual void AddRef() = 0;
};

#ifdef _WINDOWS
	#include <Windows.h>
#else
	#include <sys/epoll.h>
#endif // _WINDOWS

#ifdef _WINDOWS
/**
*名称：异步IO完成对象
*说明：功能：
		1、存放异步IO要用到的缓冲区等信息
		2、操作完成时通过它获取完成情况，例如：是否成功、传输了多少字节以及字节的内容等
*注意：每一个异步IO完成对象都必须继承一个异步IO重叠结构
*/
struct AsynIoResult : public OVERLAPPED
#else
/**
*名称：异步IO完成对象
*说明：功能：
		1、存放异步IO要用到的缓冲区等信息
		2、操作完成时通过它获取完成情况，例如：是否成功、传输了多少字节以及字节的内容等
*注意：每一个异步IO完成对象都必须继承一个epoll_event结构
*/
struct AsynIoResult : public epoll_event
#endif // _WINDOWS
{
	/**
	*作用			：设置异步IO处理器
	*参数 handler	：异步IO处理器
	*/
	virtual void SetHandler(AsynIoHandler* handler) = 0;

	/**
	*作用：通知该操作完成了
	*提示：内部调用 AsynIoHandler 处理该完成事件
	*/
	virtual void Complete() = 0;

	/**
	*作用		：准备异步IO要用的缓冲区
	*参数 nLen	：缓冲区长度
	*注意		：如果之前调用过 PrepareBuffer 准备缓冲区，
				  再次调用会将之前的缓冲区释放再重新分配
	*/
	virtual void PrepareBuffer(unsigned long nLen) = 0;

	/**
	*作用				：设置异步IO缓冲区
	*参数 pBufferList	：新缓冲区指针
	*参数 nBufferCount	：缓冲区个数
	*注意				：如果之前有分配了缓冲区，这里不会把它释放
	*/
	virtual void SetBuffer(AsynIoBuffer* pBufferList, unsigned long nBufferCount) = 0;

	/**
	*作用：获取缓冲区列表
	*返回：返回缓冲区列表指针，如果当前还没准备缓冲区，则返回nullptr
	*/
	virtual AsynIoBuffer* GetBufferList() = 0;

	/**
	*作用：获取缓冲区个数
	*返回：返回缓冲区个数，如果当前还没准备缓冲区，则返回0
	*/
	virtual unsigned long GetBufferCount() const = 0;

	/**
	*作用：获取成功传输的字节数
	*返回：如果IO没有正常完成则返回0
	*/
	virtual unsigned long GetTransferredBytes() const = 0;

	/**
	*作用		：设置成功传输的字节数
	*参数		：成功传输的字节数
	*/
	virtual void  SetTransferredBytes(unsigned long bytes) = 0;

	/**
	*作用：操作是否成功完成
	*/
	virtual bool IsSuccess() const = 0;

	/**
	*作用：获取错误码
	*返回：如果操作成功返回0
	*/
	virtual unsigned long GetErrorCode() const = 0;

	/**
	*作用：设置错误码
	*/
	virtual void SetErrorCode(unsigned long nCode) = 0;

	/**
	*作用	：获取触发该完成事件的异步IO设备指针
	*/
	virtual AsynIoDevice* GetAsynIoDevice() const = 0;

	/**
	*作用			：设置触发该完成事件的异步IO设备指针
	*参数 pDevice	：异步IO设备指针
	*/
	virtual void SetAsynIoDevice(AsynIoDevice* pDevice) = 0;

	/**
	*作用：获取异步IO操作类型
	*/
	virtual AsynIoType GetType() const = 0;

	/**
	*作用：设置异步IO操作类型
	*/
	virtual SetType(AsynIoType btType) = 0;

	/**
	*作用：释放该完成事件
	*/
	virtual void Release() = 0;
};

/**
*说明：异步IO缓冲区
*/
struct AsynIoBuffer
{
	unsigned long len;
	unsigned char *buf;
};


#endif // ASYNIOFRAME_H