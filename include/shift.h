#ifndef _SHIFT_H
#define _SHIFT_H

#include <transform.h>
#include <utils.h>

namespace avlib
{

template <class T>
class CShift : public CTransform<T, T>
{
public:
	CShift();
	CShift(T s);
	virtual void TransformBlock(T * pSrc, T * pDst, CPoint p, CSize s);
protected:
	T m_shift;
};

}


#endif //_SHIFT_H
