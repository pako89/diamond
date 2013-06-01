#ifndef _ENCODER_H
#define _ENCODER_H


#include <sequence.h>
#include <bitstream.h>
#include <mtimer.h>
#include <utils.h>
#include <avlib.h>

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
	EncoderConfig m_config;
};

}

#endif //_ENCODER_H
