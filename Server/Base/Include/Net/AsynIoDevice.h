/***********************************************************************
** �ļ�����AsynIoDevice.h
** ��  �ߣ���ˮ��(765865368@qq.com)
** ʱ  �䣺2018-11-28
** ��  �����첽IO�豸�ӿ�
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
*���ƣ��첽IO�豸
*˵����֧���첽IO������ʵ����ͨ�����ļ�����Socket
*/
struct AsynIoDevice
{
	/**
	*���ã���ȡ�豸�����Socket�����
	*/
	virtual HANDLE GetHandle() const = 0;

	/**
	*���ã���ȡ��ȡ��������豸��֧�ָò������򷵻�nullptr
	*/
	virtual AsynIoOperation_Reader* GetReader() const = 0;

	/**
	*���ã���ȡд����������豸��֧�ָò������򷵻�nullptr
	*/
	virtual AsynIoOperation_Writer* GetWriter() const = 0;

	/**
	*���ã���ȡ������������豸��֧�ָò������򷵻�nullptr
	*/
	virtual AsynIoOperation_Acceptor* GetAcceptor() const = 0;

	/**
	*���ã���ȡ������������豸��֧�ָò������򷵻�nullptr
	*/
	virtual AsynIoOperation_Connector* GetConnector() const = 0;

	/**
	*����			������һ���첽IO��ɶ���
	*���� nIoType	��IO�������ͣ�����鿴AsynIoType�Ķ���
	*���� pHandler	���첽IO������
	*����			���첽IO��ɶ���
	*��ʾ			�����첽IO�豸�����Ķ���Ӧ���ǣ�
					  1����ͬ�豸����ɶ���ʵ�ֿ������в�ͬ
					  2���豸����ʱӦ���ͷ�����δ��ɵ�Result����,�����ڴ�й¶
					  3����Ϊ�����ڶ���̵߳��ã���������ڲ��Ѿ������߳�ͬ��
	*/
	virtual AsynIoResult* GreateAsynIoResult(AsynIoType nIoType = 0, AsynIoHandler* pHandler = nullptr) = 0;

	/**
	*����			��ɾ��һ���첽IO��ɶ���
	*���� pResult	����ɶ���ָ��
	*��ʾ			����Ϊ�����ڶ���̵߳��ã���������ڲ��Ѿ������߳�ͬ��
	*/
	virtual void DeleteAsynIoResult(AsynIoResult *pResult) = 0;

	/**
	*���ã��ر��豸
	*��ʾ�����첽IOͨ��ģʽ�У������ɶ˿�Ͷ����һ���հ����󣬵��ٳ��ղ�������
		   ��ʱ�߼����ͷ��������ӣ���ʱ���ü�������Ϊ0������й©�����п���й©
		   �������������ӣ������ṩ����ӿڹ���絥�����á�
	*/
	virtual void CloseDevice() = 0;

	/**
	*����			���ͷ��첽IO�豸
	*���� bForce	���Ƿ�ǿ���ͷ�
	*��ʾ			������ʹ�������ü�����Releaseһ�����ü�����1�������ü���Ϊ0ʱ�������ͷš�
	*/
	virtual void Release(bool bForce = false) = 0;
};

#endif // ASYNIODEVICE_H