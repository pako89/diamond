#ifndef _BASIC_ENCODER_H
#define _BASIC_ENCODER_H

#include <encoder.h>
#include <proto.h>
#include <dct.h>
#include <quantizer.h>
#include <zigzag.h>
#include <rlc.h>
#include <dynamic_rlc.h>
#include <static_rlc.h>
#include <mtimer.h>
#include <prediction.h>
#include <shift.h>

namespace avlib
{

class CBasicEncoder : public CEncoder
{
public:
	CBasicEncoder();
	CBasicEncoder(EncoderConfig cfg);
	virtual ~CBasicEncoder();
protected:
	virtual void doEncodeFrame(CImage<uint8_t> * pFrame, CBitstream * pBstr, FRAME_TYPE frame_type);
	virtual void init(CImageFormat fmt);
	virtual void printTimers(void);
	CImage<float> * m_imgF;
	CImage<float> * m_imgLast;
	CImage<int16_t> * m_img;
	CDCT * m_dct;
	CQuant * m_quant;
	CZigZag<float, int16_t> * m_zz;
	CRLC<int16_t> * m_rlc;
	CShift<float> * m_shift;
	CShift<float> * m_ishift;
	CIDCT * m_idct;
	CIQuant * m_iquant;
	CPrediction * m_pred;
	CPredictionInfoTable * m_predTab;
};

}

#endif //_BASIC_ENCODER_H
