#include <transform.h>
#include <stdint.h>
#include <utils.h>

namespace avlib
{

template <class S, class D>
CTransform<S, D>::CTransform()
{
}

template <class S, class D>
CTransform<S, D>::~CTransform()
{
}

template <class S, class D>
void CTransform<S, D>::TransformBlock(S * pSrc, D * pDst, CPoint p, CSize s)
{
	for(int y=0;y<8;y++)
	{
		for(int x=0;x<8;x++)
		{
			int i = (p.Y+y)*s.Width+p.X+x;	
			pDst[i] = (D)pSrc[i]; 
		}
	}
}


template <class S, class D>
void CTransform<S, D>::Transform(CImage<S> * src, CImage<D> * dst)
{
	m_timer.start();
	doTransform(src, dst);
	m_timer.stop();
}

template <class S, class D>
void CTransform<S, D>::doTransform(CImage<S> * src, CImage<D> * dst)
{
	if(src->getFormat() != dst->getFormat())
	{
		throw utils::StringFormatException("formats does not match\n");
	}
	for(int k=0;k<src->getComponents(); k++)
	{
		CSize size((*src)[k].getHeight(), (*src)[k].getWidth());
		for(int y=0; y < (*src)[k].getHeight(); y+=8)
		{
			for(int x=0; x < (*src)[k].getWidth(); x+=8)
			{
				TransformBlock(&(*src)[k][0][0], &(*dst)[k][0][0], CPoint(k, y, x), size);
			}
		}
	}
}

INSTANTIATE2(CTransform, float, float);
INSTANTIATE2(CTransform, float, int16_t);
INSTANTIATE2(CTransform, int16_t, float);
INSTANTIATE2(CTransform, float, int32_t);
INSTANTIATE2(CTransform, int32_t, float);
INSTANTIATE2(CTransform, int16_t, int16_t);

}
