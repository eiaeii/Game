/******************************************************************************
* ��Ȩ��
* ���ߣ���ˮ��
* ˵��������ģʽ�ĵ���ģ�棬�̰߳�ȫ
******************************************************************************/

#ifndef _Singleton_HPP
#define _Singleton_HPP

template <typename T, bool mustDelete = true>
class CSingleton
{
public:
	static T* Instance()
	{
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

template<typename T, bool mustDelete> T* CSingleton<T, mustDelete>::m_spInstance = new T;

#endif // _Singleton_HPP
