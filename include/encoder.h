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
#if USE(INTERPOLATION)
	int InterpolationScale;
#endif
	bool PrintProgressBar;
	bool PrintTimers;
};

class CEncoder : public utils::ITimer
{
public:
	CEncoder();
	CEncoder(EncoderConfig cfg);
	virtual ~CEncoder();
	virtual bool Encode(CSequence * pSeq, CBitstream * pBstr);
protected:
	virtual void doEncodeFrame(CImage<uint8_t> * pFrame, CBitstream * pBstr, FRAME_TYPE frame_type) = 0;
	virtual void printTimers(void);
	virtual void printProgressBar(int i, int n);
	virtual void init(CImageFormat fmt);
	virtual sos_marker_t write_sos(CSequence * pSeq, CBitstream * pBtr);
	virtual sof_marker_t write_sof(CBitstream * pBstr, FRAME_TYPE frame_type);
	EncoderConfig m_config;
};

}

#endif //_ENCODER_H
