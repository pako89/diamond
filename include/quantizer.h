#ifndef _QUANTIZER_H
#define _QUANTIZER_H

#include <image.h>
#include <stdint.h>
#include <transform.h>

namespace avlib
{

class CQuant : public CTransform<float, float>
{
public:
	CQuant();
	CQuant(int qp);
	virtual ~CQuant();
	virtual void TransformBlock(float * pSrc, float * pDst, CPoint p, CSize s);
	virtual void setTables(const uint8_t * YQ, const uint8_t * UQ, const uint8_t * VQ, int qp);
protected:
	CImage<float> m_q;
};

class CIQuant : public CQuant
{
public:
	CIQuant();
	CIQuant(int qp);
	virtual void setTables(const uint8_t * YQ, const uint8_t * UQ, const uint8_t * VQ, int qp);
protected:
};

}

#endif //_QUANTIZER_H
