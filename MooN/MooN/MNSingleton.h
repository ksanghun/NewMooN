#pragma once
template <class T> class CMNSingleton
{
private:
	CMNSingleton();
	CMNSingleton(const CMNSingleton & rhs);
	static T* m_pInstance;

public:
	static T* GetInstance();
	static void Destory();
};

template<class T> T* CMNSingleton<T>::m_pInstance = 0;
template<class T> T* CMNSingleton<T>::GetInstance()
{
	if (m_pInstance == 0) {
		m_pInstance = new T;
	}
	return m_pInstance;
}

template<class T> void CMNSingleton<T>::Destory()
{
	if (m_pInstance) {
		delete m_pInstance;
		m_pInstance = 0;
	}
}

