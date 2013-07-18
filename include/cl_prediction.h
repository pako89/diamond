#ifndef _CL_PREDICTION_H
#define _CL_PREDICTION_H

#include <utils.h>
#include <avlib.h>
#include <prediction.h>
#include <cl_device.h>
#include <cl_image.h>
#include <cl_component.h>
#include <cl_kernel.h>

namespace avlib
{

typedef CCLComponent<prediction_info_t> CCLPredictionInfoTable;

class CCLPrediction : public CPrediction
{
public:
	CCLPrediction(CCLDevice * dev);
	virtual ~CCLPrediction();
	virtual void setTransformKernel(cl_program program, const char * kernel);
	virtual void setITransformKernel(cl_program program, const char * kernel);
	virtual void setPredictionKernel(cl_program program, const char * kernel);
	virtual CCLImage<float> * getLastImage();
	virtual void Init(CImageFormat format, int scale, cl_program program, const char * interpolation_kernel);
protected:
	virtual void doTransformPFrame(CImage<float> * pSrc, CImage<float> * pDst, CPredictionInfoTable * pPred);
	virtual void doITransformPFrame(CImage<float> * pSrc, CImage<float> * pDst, CPredictionInfoTable * pPred);
	virtual void doPredict(CComponent<float> * pSrc, CPredictionInfoTable * pPred);
	virtual void clTransform(CCLKernel * kernel, CImage<float> * pSrc, CImage<float> * pDst, CPredictionInfoTable * pPred);
	virtual void clPredict(CCLComponent<float> * pSrc, CCLComponent<float> * pLast, CCLPredictionInfoTable * pPred);
	CCLDevice * m_dev;
	CCLKernel * m_kernelTransform;
	CCLKernel * m_kernelITransform;
	CCLKernel * m_kernelPrediction;
};

}

#endif //_CL_PREDICTION_H
