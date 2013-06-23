#ifndef _PREDICTION_H
#define _PREDICTION_H

#include <transform.h>
#include <avlib.h>
#include <bitstream.h>
#include <dynamic_huffman.h>
#include <utils.h>
#include <mtimer.h>
#include <clkernel.h>
#include <component.h>

#define PREDICTION_METHOD_MSE	2
#define PREDICTION_METHOD_ABS	1

#ifndef DEFAULT_PREDICTION_METHOD
#define DEFAULT_PREDICTION_METHOD	PREDICTION_METHOD_MSE
#endif

namespace avlib
{

typedef CComponent<prediction_info_t> CPredictionInfoTable;

class CPrediction 
{
public:
	CPrediction();
	CPrediction(CImageFormat format);
	virtual ~CPrediction();
	virtual void Transform(CImage<float> * pSrc, CImage<float> * pDst, CPredictionInfoTable * pPred, FRAME_TYPE type);
	virtual void ITransform(CImage<float> * pSrc, CImage<float> * pDst, CPredictionInfoTable * pPred, FRAME_TYPE type);
	virtual void TransformBlock(float * pSrc, float * pDst, CPoint p, CSize s, prediction_info_t predInfo, int scale);
	virtual void ITransformBlock(float * pSrc, float * pDst, CPoint p, CSize s, prediction_info_t predInfo, int scale);
	virtual void setIFrameTransform(CTransform<float, float> * t);
	virtual void setIFrameITransform(CTransform<float, float> * t);
	virtual void Encode(CPredictionInfoTable * pPred, CBitstream * pBstr);
	virtual void Decode(CPredictionInfoTable * pPred, CBitstream * pBstr);
protected:
	virtual void doTransformPFrame(CImage<float> * pSrc, CImage<float> * pDst, CPredictionInfoTable * pPred);
	virtual void doITransformPFrame(CImage<float> * pSrc, CImage<float> * pDst, CPredictionInfoTable * pPred);
	virtual void doPredict(CComponent<float> * pSrc, CPredictionInfoTable * pPred);
	virtual void encodePredictionInfo(prediction_info_t, CBitstream * pBstr);
	virtual prediction_info_t decodePredictionInfo(CBitstream * pBstr);
	virtual prediction_info_t predict(float * pSrc, CPoint p, CSize s);
	float diff_abs(float * src, float * ref, CSize s);
	float diff_mse(float * src, float * ref, CSize s);
	CImage<float> * m_last;
	CTransform<float, float> * m_IFT;
	CTransform<float, float> * m_IFIT;
	int m_max;
	CDynamicHuffman<int> * m_huff;
};

}

#endif //_PREDICTION_H
