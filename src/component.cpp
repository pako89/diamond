#include <component.h>

using namespace avlib;

namespace avlib
{

template <class T>
CComponent<T>::CComponent(void) : 
	m_data(NULL),
	m_width(0),
	m_height(0)
{
}

template <class T>
CComponent<T>::CComponent(int height, int width) :
	m_data(NULL),
	m_width(0),
	m_height(0)
{
	setSize(height, width);
}

template <class T>
CComponent<T>::~CComponent(void)
{
	release();
}

template <class T>
void CComponent<T>::release(void)
{
	if(NULL != m_data)
	{
		for(int i=0;i<m_height;i++)
		{
			delete [] m_data[i];
		}
		delete [] m_data;
	}
	m_data = NULL;
	m_width = 0;
	m_height = 0;
}

template <class T>
int CComponent<T>::getWidth(void)
{
	return m_width;
}

template <class T>
int CComponent<T>::getHeight(void)
{
	return m_height;
}

template <class T>
bool CComponent<T>::setSize(int height, int width)
{
	if(height <= 0 || width <=0)
	{
		return false;
	}
	if(NULL != m_data)
	{
		release();
	}
	m_height = height;
	m_width = width;
	m_data = new T*[m_height];
	for(int i=0;i<m_height;i++)
	{
		m_data[i] = new T[m_width];
	}
	return true;
}

template <class T>
T * CComponent<T>::operator[](int height)
{
	return m_data[height];
}

template class CComponent<int>;
template class CComponent<float>;

}

