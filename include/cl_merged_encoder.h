#ifndef _CL_MERGED_ENCODER_H
#define _CL_MERGED_ENCODER_H

#include <avlib.h>
#include <encoder.h>
#include <cl_host.h>
#include <cl_prediction.h>
#include <static_rlc.h>
#include <dynamic_rlc.h>
#include <cl_shift.h>
#include <cl_dctqzz.h>

namespace avlib
{

class CCLMergedEncoder : public CEncoder, public ICLHost
{
public:
	CCLMergedEncoder();
	CCLMergedEncoder(EncoderConfig cfg);
	virtual ~CCLMergedEncoder();
protected:	
	virtual void printTimers(void);
	virtual void doEncodeFrame(CImage<uint8_t> * pFrame, CBitstream * pBstr, FRAME_TYPE frame_type);
	virtual void init(CImageFormat fmt);
	CCLDevicePolicy * m_clPolicy;
	CCLPrediction * m_pred;
	CCLPredictionInfoTable * m_predTab;
	CCLImage<float> * m_imgF;
	CCLImage<int16_t> * m_img;
	CRLC<int16_t> * m_rlc;
	CCLShift<float> * m_shift;
	CCLShift<float> * m_ishift;
	CCLDCTQZZ * m_dctqzz;
	CCLIDCTQ * m_idctq;
};

}

#endif //_CL_MERGED_ENCODER_H
