#ifndef _TRANSFORM_H
#define _TRANSFORM_H

#include <image.h>
#include <mtimer.h>

namespace avlib
{

template <class S, class D> 
class CTransform : public utils::ITimer
{
public:
	CTransform();
	~CTransform();
	virtual void TransformBlock(S * pSrc, D * pDst, CPoint p, CSize s);
	virtual void Transform(CImage<S> * src, CImage<D> * dst);
protected:
	virtual void doTransform(CImage<S> * src, CImage<D> * dst);
};

}

#endif //_TRANSFORM_H
