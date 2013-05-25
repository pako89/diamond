#ifndef _BASIC_ENCODER_H
#define _BASIC_ENCODER_H

#include <encoder.h>
#include <proto.h>
#include <dynamic_huffman.h>
#include <dct.h>
#include <quantizer.h>
#include <zigzag.h>
#include <rlc.h>
#include <dynamic_rlc.h>
#include <static_rlc.h>
#include <mtimer.h>

namespace avlib
{

class CBasicEncoder : public CEncoder
{
public:
	CBasicEncoder();
	CBasicEncoder(EncoderConfig cfg);
	virtual ~CBasicEncoder();
	virtual bool Encode(CSequence * pSeq, CBitstream * pBstr);
protected:
	virtual void init(CImageFormat fmt);
	virtual sos_marker_t write_sos(CSequence * pSeq, CBitstream * pBtr);
	virtual sof_marker_t write_sof(CBitstream * pBstr, frame_type_t frame_type);
	CImage<float> * m_imgF;
	CImage<float> * m_imgLast;
	CImage<int16_t> * m_img;
	CDynamicHuffman<int16_t> * m_htree;
	CDCT * m_dct;
	CQuant * m_quant;
	CZigZag<float, int16_t> * m_zz;
	CRLC<int16_t> * m_rlc;
};

}

#endif //_BASIC_ENCODER_H
