#include <shift.h>


namespace avlib
{

template <class T>
CShift<T>::CShift() :
	m_shift(0)
{
}

template <class T>
CShift<T>::CShift(T s) :
       m_shift(s)
{
}	

template <class T>
void CShift<T>::TransformBlock(T * pSrc, T * pDst, CPoint p, CSize s)
{
	if(m_shift)
	{
		T * src = &pSrc[p.Y*s.Width+p.X];
		T * dst = &pDst[p.Y*s.Width+p.X];
		for(int y=0;y<8;y++)
		{
			for(int x=0;x<8;x++)
			{
				dst[y*s.Width+x] = src[y*s.Width+x]+m_shift;
			}
		}
	}
}

INSTANTIATE(CShift, float);

}
