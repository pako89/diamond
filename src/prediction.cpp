#include <prediction.h>

namespace avlib
{

CPrediction::CPrediction() : 
	m_last(NULL)
{
}

CPrediction::CPrediction(CImageFormat format)
{
	m_last = new CImage<float>(format);
}

CPrediction::~CPrediction()
{
	if(NULL != m_last)
	{
		delete m_last;
	}
}

void CPrediction::Transform(CImage<float> * src, CImage<float> * dst)
{
}

void CPrediction::TransformBlock(float * pSrc, float * pDst, CPoint p, CSize s)
{
}

CIPrediction::CIPrediction() : 
	m_last(NULL)
{
}

CIPrediction::CIPrediction(CImageFormat format)
{
	m_last = new CImage<float>(format);
}

CIPrediction::~CIPrediction()
{
	if(NULL != m_last)
	{
		delete m_last;
	}
}

void CIPrediction::Transform(CImage<float> * src, CImage<float> * dst)
{
}

void CIPrediction::TransformBlock(float * pSrc, float * pDst, CPoint p, CSize s)
{
}

};
