/***********************************************************************
** �ļ�����AsynIoFrame.h
** ��  �ߣ���ˮ��(765865368@qq.com)
** ʱ  �䣺2018-11-28
** ��  �����첽IO���
************************************************************************/
#ifndef ASYNIOFRAME_H
#define ASYNIOFRAME_H

// ���Buff��Ŀ
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
*���ƣ��첽IO������
*˵���������÷���
		1������һ���첽IO����ʱ��ָ��һ���첽IO������
		2�����첽IO�������ʱ������֮ǰָ���Ĵ���������IO����
*/
struct AsynIoHandler
{
	/**
	*����			���첽IO���ʱ�ص��ú�����������д������룩
	*���� pResult	���첽IO��ɶ���ͨ�������Ի�ȡIO����������
					  �����Ƿ�ɹ��������˶����ֽ��Լ��ֽڵ����ݵ�
	*/
	virtual void HandleIoComplete(AsynIoResult* pResult) = 0;

	/**
	*���ã��ͷ�����
	*��ʾ������AsynIoHandler���ܱ�����̵߳��ã����Ա���֧�����ü������ƣ�
		   �������Ļ������⣬��Ϊ�㲻֪���Ƿ����������߳���������
	*/
	virtual void ReleaseRef() = 0;

	/**
	*���ã��������ü���
	*��ʾ������AsynIoHandler���ܱ�����̵߳��ã����Ա���֧�����ü������ƣ�
		   �������Ļ������⣬��Ϊ�㲻֪���Ƿ����������߳���������
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
*���ƣ��첽IO��ɶ���
*˵�������ܣ�
		1������첽IOҪ�õ��Ļ���������Ϣ
		2���������ʱͨ������ȡ�����������磺�Ƿ�ɹ��������˶����ֽ��Լ��ֽڵ����ݵ�
*ע�⣺ÿһ���첽IO��ɶ��󶼱���̳�һ���첽IO�ص��ṹ
*/
struct AsynIoResult : public OVERLAPPED
#else
/**
*���ƣ��첽IO��ɶ���
*˵�������ܣ�
		1������첽IOҪ�õ��Ļ���������Ϣ
		2���������ʱͨ������ȡ�����������磺�Ƿ�ɹ��������˶����ֽ��Լ��ֽڵ����ݵ�
*ע�⣺ÿһ���첽IO��ɶ��󶼱���̳�һ��epoll_event�ṹ
*/
struct AsynIoResult : public epoll_event
#endif // _WINDOWS
{
	/**
	*����			�������첽IO������
	*���� handler	���첽IO������
	*/
	virtual void SetHandler(AsynIoHandler* handler) = 0;

	/**
	*���ã�֪ͨ�ò��������
	*��ʾ���ڲ����� AsynIoHandler ���������¼�
	*/
	virtual void Complete() = 0;

	/**
	*����		��׼���첽IOҪ�õĻ�����
	*���� nLen	������������
	*ע��		�����֮ǰ���ù� PrepareBuffer ׼����������
				  �ٴε��ûὫ֮ǰ�Ļ������ͷ������·���
	*/
	virtual void PrepareBuffer(unsigned long nLen) = 0;

	/**
	*����				�������첽IO������
	*���� pBufferList	���»�����ָ��
	*���� nBufferCount	������������
	*ע��				�����֮ǰ�з����˻����������ﲻ������ͷ�
	*/
	virtual void SetBuffer(AsynIoBuffer* pBufferList, unsigned long nBufferCount) = 0;

	/**
	*���ã���ȡ�������б�
	*���أ����ػ������б�ָ�룬�����ǰ��û׼�����������򷵻�nullptr
	*/
	virtual AsynIoBuffer* GetBufferList() = 0;

	/**
	*���ã���ȡ����������
	*���أ����ػ����������������ǰ��û׼�����������򷵻�0
	*/
	virtual unsigned long GetBufferCount() const = 0;

	/**
	*���ã���ȡ�ɹ�������ֽ���
	*���أ����IOû����������򷵻�0
	*/
	virtual unsigned long GetTransferredBytes() const = 0;

	/**
	*����		�����óɹ�������ֽ���
	*����		���ɹ�������ֽ���
	*/
	virtual void  SetTransferredBytes(unsigned long bytes) = 0;

	/**
	*���ã������Ƿ�ɹ����
	*/
	virtual bool IsSuccess() const = 0;

	/**
	*���ã���ȡ������
	*���أ���������ɹ�����0
	*/
	virtual unsigned long GetErrorCode() const = 0;

	/**
	*���ã����ô�����
	*/
	virtual void SetErrorCode(unsigned long nCode) = 0;

	/**
	*����	����ȡ����������¼����첽IO�豸ָ��
	*/
	virtual AsynIoDevice* GetAsynIoDevice() const = 0;

	/**
	*����			�����ô���������¼����첽IO�豸ָ��
	*���� pDevice	���첽IO�豸ָ��
	*/
	virtual void SetAsynIoDevice(AsynIoDevice* pDevice) = 0;

	/**
	*���ã���ȡ�첽IO��������
	*/
	virtual AsynIoType GetType() const = 0;

	/**
	*���ã������첽IO��������
	*/
	virtual SetType(AsynIoType btType) = 0;

	/**
	*���ã��ͷŸ�����¼�
	*/
	virtual void Release() = 0;
};

/**
*˵�����첽IO������
*/
struct AsynIoBuffer
{
	unsigned long len;
	unsigned char *buf;
};


#endif // ASYNIOFRAME_H