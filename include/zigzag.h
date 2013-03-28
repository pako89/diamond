#ifndef _ZIGZAG_H
#define _ZIGZAG_H

#include <stdint.h>
#include <avlib.h>
#include <transform.h>

namespace avlib
{

template <class S, class D>
class CZigZag : public CTransform<S, D>
{
public:
	CZigZag();
	virtual ~CZigZag();
	virtual void TransformBlock(S * pSrc, D * pDst, CPoint p, CSize s);
protected:
	const uint8_t * m_lut;
	virtual CPoint getPoint(CPoint p);
	static const uint8_t s_lut[];
	static const uint8_t s_tul[];
};

template <class S, class D>
class CIZigZag : public CZigZag<S, D>
{
public:
	CIZigZag();
};

}

#endif //_ZIGZAG_H
