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
void CTransform<S, D>::Transform(CImage<S> & src, CImage<D> & dst)
{
	for(int k=0;k<src.getComponents(); k++)
	{
		for(int y=0; y < src[k].getHeight(); y+=8)
		{
			for(int x=0; x < src[k].getWidth(); x+=8)
			{
				Transform8x8(&src[k][y][x], &dst[k][y][x], src[k].getWidth());
			}
		}
	}
}

INSTANTIATE2(CTransform, uint8_t, float);
INSTANTIATE2(CTransform, float, uint8_t);
INSTANTIATE2(CTransform, uint16_t, float);
INSTANTIATE2(CTransform, float, uint16_t);
INSTANTIATE2(CTransform, float, float);

}
