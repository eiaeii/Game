/***********************************************************************
** ���ߣ���ˮ��(765865368@qq.com)
** ʱ�䣺2017��10��16��
** ����������ģʽ
************************************************************************/

#ifndef _Singleton_HPP
#define _Singleton_HPP
#include<stdlib.h>

template <typename T, bool mustDelete = true>
class CSingleton
{
public:
	static T* Instance()
	{
		if (nullptr == m_spInstance)
			m_spInstance = new T;

		return m_spInstance;
	}

protected:
	CSingleton()
	{
		if (mustDelete)
			atexit(CSingleton::DestroyInstance);
	}

	static void DestroyInstance()
	{
		if (nullptr != m_spInstance && mustDelete)
		{
			delete m_spInstance;
			m_spInstance = nullptr;
		}
	}
private:
	static T* m_spInstance;
};

template<typename T, bool mustDelete> T* CSingleton<T, mustDelete>::m_spInstance = nullptr;

#endif // _Singleton_HPP
