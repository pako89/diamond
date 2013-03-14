#include <image.h>
#include <utils.h>

namespace avlib
{

template <class T>
CImage<T>::CImage() :
	m_comp_num(0),
	m_comp(NULL)
{
}

template <class T>
CImage<T>::CImage(enum ImageType type, int height, int width) :
	m_comp_num(0),
	m_comp(NULL)
{
	setSize(type, height, width);
}

template <class T>
CImage<T>::~CImage()
{
}

template <class T>
bool CImage<T>::setSize(enum ImageType type, int height, int width)
{
	if(NULL != m_comp)
	{
		delete [] m_comp;
	}
	switch(type)
	{
	case IMAGE_TYPE_YUV420:
		m_comp = new CComponent<T>[3];
		m_comp_num = 3;
		m_comp[0].setSize(height, width);
		m_comp[1].setSize(height/2, width/2);
		m_comp[2].setSize(height/2, width/2);
	break;
	default:
		return false;
	}
	return true;
}

template <class T>
int CImage<T>::getComponents(void)
{
	return m_comp_num;
}

template <class T>
CComponent<T> & CImage<T>::operator[](int index)
{
	if(index >= 0 && index < m_comp_num)
	{
		return m_comp[index];
	}
	else
	{
		throw std::exception();
	}
}

INSTANTIATE(CImage, uint8_t);
INSTANTIATE(CImage, int);
INSTANTIATE(CImage, float);
}
