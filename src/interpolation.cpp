#include <interpolation.h>

namespace avlib
{

template <class T>
CInterpolation<T>::CInterpolation() :
	m_scale(2)
{
}

template <class T>
void CInterpolation<T>::doTransform(CImage<T> * src, CImage<T> * dst)
{
	if(src->getFormat().Type != dst->getFormat().Type)
	{
		throw utils::StringFormatException("Interpolation: images types are not equal");
	}
	if(src->getFormat().Size.Width*m_scale != dst->getFormat().Size.Width
	|| src->getFormat().Size.Height*m_scale != dst->getFormat().Size.Height)
	{
		throw utils::StringFormatException("Interpolation: images sizes do not match: src[%dx%d] dst[%dx%d] scale[%d]",
				src->getFormat().Size.Width,
				src->getFormat().Size.Height,
				dst->getFormat().Size.Width,
				dst->getFormat().Size.Height,
				m_scale
				);
	}
	for(int i=0;i<dst->getComponents(); i++)
	{
		for(int y=0;y<(*dst)[i].getHeight();y++)
		{
			for(int x=0;x<(*dst)[i].getWidth();x++)
			{
				if(y%m_scale == 0 && x%m_scale == 0)
				{
					(*dst)[i][y][x] = (*src)[i][y/m_scale][x/m_scale];
				}
				else
				{
					/*TODO*/
					(*dst)[i][y][x] = (*src)[i][y/m_scale][x/m_scale];
				}
			}
		}
	}
}

template <class T>
void CInterpolation<T>::setScale(int scale)
{
	if(scale == 2 || scale == 1)
	{
		m_scale = scale;
	}
	else
	{
		throw utils::StringFormatException("wrong scale for interpolation: '%d'", scale);
	}
}

template <class T>
int CInterpolation<T>::getScale(void)
{
	return m_scale;
}

INSTANTIATE(CInterpolation, float);
INSTANTIATE(CInterpolation, int16_t);

}
