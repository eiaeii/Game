/***********************************************************************
** 作者：何水大(765865368@qq.com)
** 时间：2017年10月16日
** 描述：单例模式
************************************************************************/

#ifndef _Singleton_HPP
#define _Singleton_HPP
#include<stdlib.h>

template <typename T, bool mustDelete = true>
class CSingleton
{
public:
	static T* GetInstance()
	{
		if (nullptr == m_spGetInstance)
			m_spGetInstance = new T;

		return m_spGetInstance;
	}

protected:
	CSingleton()
	{
		if (mustDelete)
			atexit(CSingleton::DestroyGetInstance);
	}

	static void DestroyGetInstance()
	{
		if (nullptr != m_spGetInstance && mustDelete)
		{
			delete m_spGetInstance;
			m_spGetInstance = nullptr;
		}
	}
private:
	static T* m_spGetInstance;
};

template<typename T, bool mustDelete> T* CSingleton<T, mustDelete>::m_spGetInstance = nullptr;

#endif // _Singleton_HPP
