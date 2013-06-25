#ifndef _ENCODER_H
#define _ENCODER_H


#include <sequence.h>
#include <bitstream.h>
#include <mtimer.h>
#include <utils.h>
#include <avlib.h>
#include <proto.h>

namespace avlib
{

struct EncoderConfig
{
	HUFFMAN_TYPE HuffmanType;
	int GOP;
};

class CEncoder : public utils::ITimer
{
public:
	CEncoder();
	CEncoder(EncoderConfig cfg);
	virtual ~CEncoder();
	virtual bool Encode(CSequence * pSeq, CBitstream * pBstr) = 0;
protected:
	virtual sos_marker_t write_sos(CSequence * pSeq, CBitstream * pBtr);
	virtual sof_marker_t write_sof(CBitstream * pBstr, FRAME_TYPE frame_type);
	EncoderConfig m_config;
};

}

#endif //_ENCODER_H
