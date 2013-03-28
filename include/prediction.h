#ifndef _PREDICTION_H
#define _PREDICTION_H

#include <transform.h>

namespace avlib
{

class CPrediction : public CTransform<float, float>
{
public:
	CPrediction();
	CPrediction(CImageFormat format);
	virtual ~CPrediction();
	virtual void Transform(CImage<float> * src, CImage<float> * dst);
	virtual void TransformBlock(float * pSrc, float * pDst, CPoint p, CSize s);
protected:
	CImage<float> * m_last;
};

class CIPrediction : public CTransform<float, float>
{
public:
	CIPrediction();
	CIPrediction(CImageFormat format);
	virtual ~CIPrediction();
	virtual void Transform(CImage<float> * src, CImage<float> * dst);
	virtual void TransformBlock(float * pSrc, float * pDst, CPoint p, CSize s);
protected:
	CImage<float> * m_last;
};

}

#endif //_PREDICTION_H
