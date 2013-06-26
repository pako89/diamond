#ifndef _CL_PARALLEL_ENCODER_H
#define _CL_PARALLEL_ENCODER_H

#ifndef DEFAULT_CL_SRC_FILE
#define DEFAULT_CL_SRC_FILE "kernel.cl"
#endif

#include <avlib.h>
#include <utils.h>
#include <encoder.h>
#include <cl_host.h>
#include <cl_dct.h>
#include <cl_quantizer.h>
#include <cl_zigzag.h>
#include <rlc.h>
#include <static_rlc.h>
#include <dynamic_rlc.h>
#include <cl_shift.h>
#include <cl_prediction.h>
#include <cl_image.h>

namespace avlib
{

class CCLParallelEncoder : public CEncoder, public ICLHost
{
public:
	CCLParallelEncoder();
	CCLParallelEncoder(EncoderConfig cfg);
	virtual ~CCLParallelEncoder();
	virtual bool Encode(CSequence * pSeq, CBitstream * pBstr);
protected:	
	virtual void init(CImageFormat fmt);
	virtual void transform(CCLImage<float> * imgF, CCLImage<int16_t> * img, CCLPredictionInfoTable * predInfo, FRAME_TYPE frame_type);
	virtual void itransform(CCLImage<float> * imgF, CCLImage<int16_t> * img, CCLPredictionInfoTable * predInfo, FRAME_TYPE frame_type);
	virtual void entropy(CCLImage<int16_t> * img, CCLPredictionInfoTable * predInfo, CBitstream * pBstr);
	CCLDevicePolicy * m_clPolicy;
	CCLDCT * m_dct;
	CCLIDCT * m_idct;
	CCLQuant * m_quant;
	CCLIQuant * m_iquant;
	CCLZigZag<float, int16_t> * m_zz;
	CCLShift<float> * m_shift;
	CCLShift<float> * m_ishift;
	CCLPrediction * m_pred;
	CCLPredictionInfoTable * m_predInfo;
	CRLC<int16_t> * m_rlc;
	CCLImage<float> * m_imgF;
	CCLImage<int16_t> * m_img;
	//utils::CTimer m_timerCopyToDevice;
	//utils::CTimer m_timerCopyToHost;
};

}

#endif // _CL_PARALLEL_ENCODER_H
