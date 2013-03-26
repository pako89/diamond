#include <component.h>
#include <utils.h>

using namespace avlib;

namespace avlib
{

template <class T>
CComponent<T>::CComponent(void) : 
	m_data(NULL),
	m_size(0, 0)
{
}

template <class T>
CComponent<T>::CComponent(CSize size) :
	m_data(NULL),
	m_size(0, 0)
{
	setSize(size);
}


template <class T>
CComponent<T>::CComponent(int height, int width) :
	m_data(NULL),
	m_size(0, 0)
{
	setSize(height, width);
}

template <class T>
CComponent<T>::CComponent(const CComponent<T> & src) :
	m_data(NULL),
	m_size(0, 0)
{
	operator=(src);
}

template <class T>
CComponent<T> & CComponent<T>::operator=(const CComponent & src)
{
	if(m_size != src.m_size)
	{
		setSize(src.m_size.Height, src.m_size.Width);
	}
	for(int i=0;i<getPointsCount();i++)
	{
		m_data[i] = (T)src.m_data[i];
	}
	return *this;
}

template <class T>
template <class U> CComponent<T>::CComponent(const CComponent<U> & src) :
	m_data(NULL),
	m_size(0, 0)
{
	operator=(src);
}

template <class T>
template <class U> CComponent<T> & CComponent<T>::operator=(const CComponent<U> & src)
{
	if(m_size != src.m_size)
	{
		setSize(src.m_size.Height, src.m_size.Width);
	}
	for(int i=0;i<getPointsCount();i++)
	{
		m_data[i] = (T)src.m_data[i];
	}
	return *this;
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
	m_size.Width = 0;
	m_size.Height = 0;
}

template <class T>
int CComponent<T>::getWidth(void)
{
	return m_size.Width;
}

template <class T>
int CComponent<T>::getHeight(void)
{
	return m_size.Height;
}

template <class T>
int CComponent<T>::getPointsCount(void)
{
	return m_size.Height*m_size.Width;
}

template <class T>
CSize CComponent<T>::getSize(void)
{
	return m_size;
}

template <class T>
size_t CComponent<T>::getBytesCount(void)
{
	return m_bytes;
}


template <class T>
bool CComponent<T>::setSize(int height, int width)
{
	return setSize(CSize(height, width));
}

template <class T>
bool CComponent<T>::setSize(CSize size)
{
	if(size.Height <= 0 || size.Width <=0)
	{
		return false;
	}
	if(NULL != m_data)
	{
		release();
	}
	m_size = size;
	m_bytes = m_size.Height*m_size.Width*sizeof(T);
	m_data = new T[m_size.Height*m_size.Width];
	return true;
}

template <class T>
T * CComponent<T>::operator[](int h)
{
	return &m_data[h * m_size.Width];
}

template <class T>
CComponent<T> & CComponent<T>::operator-=(const CComponent<T> & src)
{
	if(m_size != src.m_size)
	{
		throw utils::StringFormatException("wrong sizes\n");
	}
	int count = getPointsCount();
	for(int i=0;i<count;i++)
	{
		m_data[i] -= src.m_data[i];
	}

	return *this;
}

template <class T>
CComponent<T> & CComponent<T>::operator+=(const CComponent<T> & src)
{
	if(m_size != src.m_size)
	{
		throw utils::StringFormatException("wrong sizes\n");
	}
	int count = getPointsCount();
	for(int i=0;i<count;i++)
	{
		m_data[i] += src.m_data[i];
	}

	return *this;
}

INSTANTIATE(CComponent, uint8_t);
INSTANTIATE(CComponent, uint16_t);
INSTANTIATE(CComponent, int16_t);
INSTANTIATE(CComponent, int32_t);
INSTANTIATE(CComponent, float);
CONVERSION(CComponent, int16_t, float);
CONVERSION(CComponent, float, int16_t);
CONVERSION(CComponent, int32_t, float);
CONVERSION(CComponent, float, int32_t);
CONVERSION(CComponent, uint8_t, float);
CONVERSION(CComponent, float, uint8_t);
CONVERSION(CComponent, uint16_t, uint8_t);
CONVERSION(CComponent, uint8_t, uint16_t);
}

