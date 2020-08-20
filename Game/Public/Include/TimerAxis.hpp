/***********************************************************************
** ���ߣ���ˮ��(765865368@qq.com)
** ʱ�䣺2020-08-17
** ��������ʱ��
************************************************************************/

#pragma once

#include <stdlib.h>
#include "Singleton.hpp"
#include <string>
#include <list>
#include <vector>

/*
*  ����:
*  1.OnCheck����Ƶ�ʴ��ڵ�λʱ��̶�ʱ��ô����?
*  2.���߳�֧��
*  3.��Timer��ô����?
*  4.��Ҫ֧�ֵ��Թ���
*
*  ����:
*  1.ʱ����ĺ���ʵ����һ����Ч�������㷨
*  2.���㷨�Բ���ɾ������Ҫ�󼫸�
*  3.��Ҫ�ǲ�������,���Ƕ�һ��������������
*  4.��������ʵ������Ҫ��Ҫ������ξ����ҵ�һ��ֵ�������ڵ�λ��
*
*  ��Ʒ���:
*  1.��ʱ����ֳ�N���̶�,ÿ���̶��ڱ���˶�ʱ������Ҫ���õ�Timer
*  2.��Timer�����N����Timer�ص�
*  3.��Ϊ�ڵ�λʱ��̶���ֻ��Ҳֻ����ü���,����ͬһ�̶��ڵ�Timer��������
*  4.����ʱ������N����ͬ���ȶε���ʱ������϶���,������ʱƵ����1������
*    ��ҪƵ�����õĶ�ʱ�����Է���һ��ÿ�񳤶�1ms���ȵ���1�뼫��ϸ����ʱ������
*    �Դﵽ���Ч��
*  5.�������ɾ��Timerʱ�Ĳ��Ҳ������Դ�����Ч��.
*/

/****** ʱ��������> ******
* �Ƽ����Ƶ�� : 16(ms) 
* �Ƽ�ʱ��̶� : 64(ms)
*/
#define CHECK_FREQUENCY	        16	//��ȷ��16ms
#define TIME_GRID				64

// ʱ���᳤��
#define TIME_AXIS_LENGTH		720000
#define INVALID_TIMER			0xFFFFFFFF
#define INFINITY_CALL			0xFFFFFFFF

// ---------------------------------------------------------------------------
// Name	: ��ʱ��������Ĵ���ӿ�
// Desc : ��Ӧ�ò�ʹ��ʱ������ITimerHandler�̳У�
// ---------------------------------------------------------------------------

class ITimerHandler
{
public:
	ITimerHandler() {};
	virtual ~ITimerHandler();
	/**
	@purpose			: ��ʱ��������ص�,������������д�������
	@param nTimerID		: ��ʱ��ID,�����������ĸ���ʱ��
	@return				: empty
	*/
	virtual void OnTimer(uint32_t nTimerID) = 0;

private:

	/**
	@purpose          : ȡ�ô�Ŷ�ʱ���ڲ����ݵ�ָ��
	@note             : ���������Ӧ�ò��޹�,����ITimerHandler�����ʵ�ּ���
	@note             : ʱ�������ʵ������Ҫ��߶�ʱ���Ĳ���ɾ��Ч�ʣ�������Ӧ�ò��¼һ��ָ��
	@return		      : ����ָ����ʱ��Ϣ��ָ��,��ָ������ʼ����nullptr
	*/

	void** GetTimerInfo()
	{
		return &m_pTimerInfo;
	}

private:
	
	// ��ʱ���ڲ����ݣ�std::list<Timer>������Timer����ض���
	void* m_pTimerInfo = nullptr;
};

class CTimerAxist : public CSingleton<CTimerAxist>
{
	struct Timer
	{
		uint32_t nTimerID; // ��ʱ��ID
		uint32_t nInterval; // �������
		uint32_t nCallTimes;// ��������
		uint64_t nLastCallTick;// ���һ�ε��õ�ʱ��
		uint32_t nGridIndex;// ���ڵ�ʱ��̶�
		ITimerHandler* pHandler; // ��ʱ���ص��ӿ�
		std::string strDebugInfo; // ������Ϣ
		std::list <Timer*>::iterator itrPos; // ��ʱ��̶��е�iterator���ӿ�����
	};

	using TIMER_INFO = std::list<Timer>; // ����ITimerHandler�еĶ�ʱ����ʱ��Ϣ
	using TIMER_LIST = std::list<Timer*>; // ÿһ��ʱ��̶��д�ŵĶ�ʱ���б�
	using TIMER_AXIS = std::vector<TIMER_LIST>; // ��������ʱ��̶���Ϣ��ʱ����ṹ

public:
	CTimerAxist();
	virtual ~CTimerAxist();

public:
	bool SetTimer(uint32_t nTimerID, uint32_t nInterval, ITimerHandler* pHandler, uint32_t nCallTimes = INFINITY_CALL, const char* pszDebugInfo = nullptr);
	bool KillTimer(uint32_t nTimerID, ITimerHandler* pHandler);
	void KillTimer(ITimerHandler* pHandler);

private:
	TIMER_AXIS m_vecTimerAxis;
	uint64_t m_nLastCheckTick; // ���һ�μ���ʱ��
	uint64_t m_nCurRunTime; // ��ǰ���е�ʱ��
	uint64_t m_InitTime; // ʱ�����ʼʱ��
	uint32_t m_nTimerCount; // ��ʱ������
};
