#ifndef _PREDICTION_H
#define _PREDICTION_H

#include <transform.h>
#include <avlib.h>
#include <bitstream.h>
#include <dynamic_huffman.h>
#include <utils.h>
#include <mtimer.h>

typedef struct prediction_info
{
	int32_t dy;
	int32_t dx;
} prediction_info_t;

namespace avlib
{

class CPredictionInfo
{
public:
	CPredictionInfo();
	CPredictionInfo(int32_t dy, int32_t dx);
	CPredictionInfo(prediction_info_t pi);
	virtual ~CPredictionInfo();
	virtual void Encode(CBitstream * pBstr);
	virtual void Decode(CBitstream * pBstr);
	virtual void Encode(CDynamicHuffman<int32_t> * pHuff, CBitstream * pBstr);
	virtual void Decode(CDynamicHuffman<int32_t> * pHuff, CBitstream * pBstr);
	int32_t deltaY();
	int32_t deltaX();
protected:
	prediction_info_t m_info;
};

class CPredictionInfoTable
{
public:
	CPredictionInfoTable();
	CPredictionInfoTable(CSize size);
	CPredictionInfoTable(const CPredictionInfoTable & src);
	virtual ~CPredictionInfoTable();
	virtual void setSize(CSize size, bool copy = true);
	CSize getSize(void) { return m_size; }
	CPredictionInfo * operator[](int index);
protected:
	virtual void release();
	CSize m_size;
	CPredictionInfo ** m_tab;
};

class CPrediction 
{
public:
	CPrediction();
	CPrediction(CImageFormat format);
	virtual ~CPrediction();
	virtual void Transform(CImage<float> * src, CImage<float> * dst, CPredictionInfoTable * pPred, FRAME_TYPE type);
	virtual void ITransform(CImage<float> * src, CImage<float> * dst, CPredictionInfoTable * pPred, FRAME_TYPE type);
	virtual void TransformBlock(float * pSrc, float * pDst, CPoint p, CSize s, CPredictionInfo predInfo);
	virtual void ITransformBlock(float * pSrc, float * pDst, CPoint p, CSize s, CPredictionInfo predInfo);
	virtual void setIFrameTransform(CTransform<float, float> * t);
	virtual void setIFrameITransform(CTransform<float, float> * t);
protected:
	CPredictionInfo predict(float * pSrc, CPoint p, CSize s);
	float diff_abs(float * src, float * ref, CSize s);
	float diff_mse(float * src, float * ref, CSize s);
	
	CImage<float> * m_last;
	CTransform<float, float> * m_IFT;
	CTransform<float, float> * m_IFIT;
	int m_max;
};

}

#endif //_PREDICTION_H
