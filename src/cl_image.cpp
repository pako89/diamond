#include <cl_image.h>

namespace avlib
{

template <class T>
CCLImage<T>::CCLImage(cl_handle h)
{}

template <class T>
CCLImage<T>::CCLImage(cl_handle h, CSize size) :
	m_clh(h)
{
	CCLImage<T>::setFormat(CImageFormat(IMAGE_TYPE_RGB, size));
}

template <class T>
CCLImage<T>::CCLImage(cl_handle h, CImageFormat format) :
	m_clh(h)
{
	CCLImage<T>::setFormat(format);
}

template <class T>
CCLImage<T>::CCLImage(cl_handle h, ImageType type, CSize size) :
	m_clh(h)
{
	CCLImage<T>::setFormat(CImageFormat(type, size));
}

template <class T>
CCLImage<T>::CCLImage(cl_handle h, ImageType type, int height, int width) :
	m_clh(h)
{
	this->setFormat(CImageFormat(type, CSize(height, width)));
}

template <class T>
void CCLImage<T>::alloc(int num)
{
	this->m_comp_num = num;
	this->m_comp = new CComponent<T>*[this->m_comp_num];
	for(int i= 0 ; i < this->m_comp_num;i++)
	{
		this->m_comp[i] = new CCLComponent<T>(m_clh);
	}
}

template <class T>
void CCLImage<T>::CopyToDevice()
{
	for(int i=0;i<this->m_comp_num; i++)
	{
		((CCLComponent<T>*)this->m_comp[i])->CopyToDevice();
	}
}

template <class T>
void CCLImage<T>::CopyToHost()
{
	for(int i=0;i<this->m_comp_num; i++)
	{
		((CCLComponent<T>*)this->m_comp[i])->CopyToHost();
	}
}

template <class T>
CCLComponent<T> & CCLImage<T>::getCLComponent(int index)
{
	return *dynamic_cast<CCLComponent<T>*>(this->m_comp[index]);
}

INSTANTIATE(CCLImage, float);

}
