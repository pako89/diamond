#ifndef _CL_ENCODER_H
#define _CL_ENCODER_H

#include <encoder.h>
#include <rlc_factory.h>
#include <cl_device.h>
#include <cl_image.h>
#include <cl_dct.h>
#include <cl_quantizer.h>
#include <cl_zigzag.h>
#include <cl_shift.h>
#include <cl_base.h>
#include <cl_policy.h>
#include <cl_prediction.h>
#include <cl_host.h>
#include <CL/opencl.h>
#include <mtimer.h>

namespace avlib
{

class CCLEncoder : public CEncoder, public ICLHost
{
public:
	CCLEncoder();
	CCLEncoder(EncoderConfig cfg);
	virtual ~CCLEncoder();
protected:
	virtual void init(CImageFormat fmt);
	virtual void printTimers(void);
	virtual void doEncodeFrame(CImage<uint8_t> * pFrame, CBitstream * pBstr, FRAME_TYPE frame_type);
	virtual void transform(CCLImage<float> * imgF, CCLImage<int16_t> * img, CCLPredictionInfoTable * predInfo, FRAME_TYPE frame_type);
	virtual void itransform(CCLImage<float> * imgF, CCLImage<int16_t> * img, CCLPredictionInfoTable * predInfo, FRAME_TYPE frame_type);
	virtual void entropy(CCLImage<int16_t> * img, CCLPredictionInfoTable * predInfo, CBitstream * pBstr, FRAME_TYPE frame_type);
	CCLDevicePolicy * m_clPolicy;
	CCLDCT * m_dct;
	CCLIDCT * m_idct;
	CCLQuant * m_quant;
	CCLIQuant * m_iquant;
	CCLZigZag<float, int16_t> * m_zz;
	CCLShift<float> * m_shift;
	CCLShift<float> * m_ishift;
	CCLPrediction * m_pred;
	CCLPredictionInfoTable * m_predTab;
	CRLC<int16_t> * m_rlc;
	CCLImage<float> * m_imgF;
	CCLImage<int16_t> * m_img;
};

}

#endif //_CL_ENCODER
