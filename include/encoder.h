#ifndef _ENCODER_H
#define _ENCODER_H


#include <sequence.h>
#include <bitstream.h>
#include <mtimer.h>
#include <utils.h>
#include <avlib.h>
#include <proto.h>

#ifndef DEFAULT_GOP
#define DEFAULT_GOP	4
#endif

#ifndef DEFAULT_PROGRESS_BAR
#define DEFAULT_PROGRESS_BAR 	true
#endif

#ifndef DEFAULT_PRINT_TIMERS
#define DEFAULT_PRINT_TIMERS	false
#endif

#ifndef DEFAULT_INTERPOLATION_SCALE
#define DEFAULT_INTERPOLATION_SCALE	2
#endif

#ifndef DEFAULT_HUFFMAN_TYPE
#define DEFAULT_HUFFMAN_TYPE	HUFFMAN_TYPE_STATIC
#endif

namespace avlib
{

struct EncoderConfig
{
	EncoderConfig() :
		HuffmanType(DEFAULT_HUFFMAN_TYPE),
		GOP(DEFAULT_GOP),
#if USE(INTERPOLATION)
		InterpolationScale(DEFAULT_INTERPOLATION_SCALE),
#endif
		PrintProgressBar(DEFAULT_PROGRESS_BAR),
		PrintTimers(DEFAULT_PRINT_TIMERS),
		Q(DEFAULT_Q)
		
	{}
	HUFFMAN_TYPE HuffmanType;
	int GOP;
	int Q;
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
