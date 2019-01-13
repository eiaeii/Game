/***********************************************************************
** 文件名：AsynIoDevice.h
** 作  者：何水大(765865368@qq.com)
** 时  间：2018-11-28
** 描  述：异步IO设备接口
************************************************************************/

#ifndef ASYNIODEVICE_H
#define ASYNIODEVICE_H

#include "CommonDefine.h"

struct AsynIoResult;
struct AsynIoHandler;
struct AsynIoOperation_Reader;
struct AsynIoOperation_Writer;
struct AsynIoOperation_Acceptor;
struct AsynIoOperation_Connector;
enum  class AsynIoType;

/**
*名称：异步IO设备
*说明：支持异步IO操作的实例，通常是文件或者Socket
*/
struct AsynIoDevice
{
	/**
	*作用：获取设备句柄，Socket对象等
	*/
	virtual HANDLE GetHandle() const = 0;

	/**
	*作用：获取读取器，如果设备不支持该操作，则返回nullptr
	*/
	virtual AsynIoOperation_Reader* GetReader() const = 0;

	/**
	*作用：获取写入器，如果设备不支持该操作，则返回nullptr
	*/
	virtual AsynIoOperation_Writer* GetWriter() const = 0;

	/**
	*作用：获取接收器，如果设备不支持该操作，则返回nullptr
	*/
	virtual AsynIoOperation_Acceptor* GetAcceptor() const = 0;

	/**
	*作用：获取连接器，如果设备不支持该操作，则返回nullptr
	*/
	virtual AsynIoOperation_Connector* GetConnector() const = 0;

	/**
	*作用			：创建一个异步IO完成对象
	*参数 nIoType	：IO操作类型，详情查看AsynIoType的定义
	*参数 pHandler	：异步IO处理器
	*返回			：异步IO完成对象
	*提示			：由异步IO设备创建的对象应考虑：
					  1、不同设备的完成对象实现可能略有不同
					  2、设备销毁时应该释放所有未完成的Result对象,避免内存泄露
					  3、因为可能在多个线程调用，这个函数内部已经做了线程同步
	*/
	virtual AsynIoResult* GreateAsynIoResult(AsynIoType nIoType = 0, AsynIoHandler* pHandler = nullptr) = 0;

	/**
	*作用			：删除一个异步IO完成对象
	*参数 pResult	：完成对象指针
	*提示			：因为可能在多个线程调用，这个函数内部已经做了线程同步
	*/
	virtual void DeleteAsynIoResult(AsynIoResult *pResult) = 0;

	/**
	*作用：关闭设备
	*提示：在异步IO通信模式中，如果完成端口投递了一个收包请求，但迟迟收不到包，
		   这时逻辑层释放网络连接，这时引用计数不会为0，导致泄漏，且有可能泄漏
		   大量的网络连接，所以提供这个接口供外界单独调用。
	*/
	virtual void CloseDevice() = 0;

	/**
	*作用			：释放异步IO设备
	*参数 bForce	：是否强制释放
	*提示			：由于使用了引用计数，Release一次引用计数减1，当引用计数为0时才真正释放。
	*/
	virtual void Release(bool bForce = false) = 0;
};

#endif // ASYNIODEVICE_H