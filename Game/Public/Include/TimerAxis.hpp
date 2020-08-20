/***********************************************************************
** 作者：何水大(765865368@qq.com)
** 时间：2020-08-17
** 描述：定时器
************************************************************************/

#pragma once

#include <stdlib.h>
#include "Singleton.hpp"
#include <string>
#include <list>
#include <vector>

/*
*  问题:
*  1.OnCheck调用频率大于单位时间刻度时怎么处理?
*  2.多线程支持
*  3.长Timer怎么处理?
*  4.需要支持调试功能
*
*  分析:
*  1.时间轴的核心实际是一个高效的排序算法
*  2.此算法对插入删除操作要求极高
*  3.主要是插入排序,不是对一堆数字整体排序
*  4.插入排序实际最主要的要求是如何尽快找到一个值大致所在的位置
*
*  设计方案:
*  1.把时间轴分成N个刻度,每个刻度内保存此段时间内需要调用的Timer
*  2.长Timer折算成N个短Timer回调
*  3.因为在单位时间刻度内只能也只需调用几次,所以同一刻度内的Timer不需排序
*  4.完整时间轴由N个不同长度段的子时间轴组合而成,这样定时频率在1秒以下
*    需要频繁调用的定时器可以放在一个每格长度1ms长度等于1秒极度细化的时间轴中
*    以达到最高效率
*  5.避免添加删除Timer时的查找操作可以大大提高效率.
*/

/****** 时间轴配置> ******
* 推荐检查频率 : 16(ms) 
* 推荐时间刻度 : 64(ms)
*/
#define CHECK_FREQUENCY	        16	//精确到16ms
#define TIME_GRID				64

// 时间轴长度
#define TIME_AXIS_LENGTH		720000
#define INVALID_TIMER			0xFFFFFFFF
#define INFINITY_CALL			0xFFFFFFFF

// ---------------------------------------------------------------------------
// Name	: 定时器触发后的处理接口
// Desc : （应用层使用时尽量从ITimerHandler继承）
// ---------------------------------------------------------------------------

class ITimerHandler
{
public:
	ITimerHandler() {};
	virtual ~ITimerHandler();
	/**
	@purpose			: 定时器触发后回调,你可以在这里编写处理代码
	@param nTimerID		: 定时器ID,用于区分是哪个定时器
	@return				: empty
	*/
	virtual void OnTimer(uint32_t nTimerID) = 0;

private:

	/**
	@purpose          : 取得存放定时器内部数据的指针
	@note             : 这个函数和应用层无关,参照ITimerHandler基类的实现即可
	@note             : 时间轴具体实现中需要提高定时器的插入删除效率，所以让应用层记录一个指针
	@return		      : 返回指向临时信息的指针,该指针必须初始化成nullptr
	*/

	void** GetTimerInfo()
	{
		return &m_pTimerInfo;
	}

private:
	
	// 定时器内部数据，std::list<Timer>，参照Timer的相关定义
	void* m_pTimerInfo = nullptr;
};

class CTimerAxist : public CSingleton<CTimerAxist>
{
	struct Timer
	{
		uint32_t nTimerID; // 定时器ID
		uint32_t nInterval; // 触发间隔
		uint32_t nCallTimes;// 触发次数
		uint64_t nLastCallTick;// 最后一次调用的时间
		uint32_t nGridIndex;// 所在的时间刻度
		ITimerHandler* pHandler; // 定时器回调接口
		std::string strDebugInfo; // 调试信息
		std::list <Timer*>::iterator itrPos; // 在时间刻度中的iterator，加快搜索
	};

	using TIMER_INFO = std::list<Timer>; // 存在ITimerHandler中的定时器临时信息
	using TIMER_LIST = std::list<Timer*>; // 每一个时间刻度中存放的定时器列表
	using TIMER_AXIS = std::vector<TIMER_LIST>; // 保存所有时间刻度信息的时间轴结构

public:
	CTimerAxist();
	virtual ~CTimerAxist();

public:
	bool SetTimer(uint32_t nTimerID, uint32_t nInterval, ITimerHandler* pHandler, uint32_t nCallTimes = INFINITY_CALL, const char* pszDebugInfo = nullptr);
	bool KillTimer(uint32_t nTimerID, ITimerHandler* pHandler);
	void KillTimer(ITimerHandler* pHandler);

private:
	TIMER_AXIS m_vecTimerAxis;
	uint64_t m_nLastCheckTick; // 最后一次检测的时间
	uint64_t m_nCurRunTime; // 当前运行的时间
	uint64_t m_InitTime; // 时间轴初始时间
	uint32_t m_nTimerCount; // 定时器个数
};
