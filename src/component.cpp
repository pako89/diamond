#include <component.h>
#include <utils.h>

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
size_t CComponent<T>::getBytesCount(void)
{
	return m_bytes;
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
	m_bytes = height*width*sizeof(T);
	m_data = new T[m_height*m_width];
	return true;
}

template <class T>
T * CComponent<T>::operator[](int h)
{
	return &m_data[h * m_width];
}

INSTANTIATE(CComponent, uint8_t);
INSTANTIATE(CComponent, int);
INSTANTIATE(CComponent, float);

}

