#ifndef _ENCODER_H
#define _ENCODER_H


#include <sequence.h>
#include <bitstream.h>
#include <mtimer.h>
#include <utils.h>
#include <avlib.h>
#include <proto.h>
#include <cl_device_info.h>

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

#ifndef DEFAULT_Q
#define DEFAULT_Q	1
#endif

#ifndef DEFAULT_DEVICE_TYPE
#define DEFAULT_DEVICE_TYPE	CL_DEVICE_TYPE_GPU
#endif

#ifndef DEFAULT_KERNEL_SRC
#define DEFAULT_KERNEL_SRC	"kernel.cl"
#endif

namespace avlib
{

struct EncoderConfig
{
	EncoderConfig() :
		HuffmanType(DEFAULT_HUFFMAN_TYPE),
		GOP(DEFAULT_GOP),
		InterpolationScale(DEFAULT_INTERPOLATION_SCALE),
		PrintProgressBar(DEFAULT_PROGRESS_BAR),
		PrintTimers(DEFAULT_PRINT_TIMERS),
		Q(DEFAULT_Q),
		Device(DEFAULT_DEVICE_TYPE),
		KernelSrc(DEFAULT_KERNEL_SRC)	
	{}
	HUFFMAN_TYPE HuffmanType;
	int GOP;
	int Q;
	int InterpolationScale;
	bool PrintProgressBar;
	bool PrintTimers;
	const char * KernelSrc;
	cl_device_type Device;
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
