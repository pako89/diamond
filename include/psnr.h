#ifndef _PSNR_H
#define _PSNR_H

#include <utils.h>
#include <log.h>
#include <sequence.h>

namespace utils
{

struct PSNRConfig
{
	struct
	{
		const char * FileName;
		FILE * File;
	} Seq[2];
	int GOP;
};

template <class T>
struct PSNRResultItem
{
	PSNRResultItem() :
		Y(0),
		U(0),
		V(0)
	{}

	T Y;
	T U;
	T V;
};

struct PSNRResult
{
	PSNRResultItem<float> Sum;
	PSNRResultItem<int> Count;
	PSNRResultItem<float> getMean();
	PSNRResult & operator+=(const PSNRResultItem<float> & r);
};

struct PSNRResults
{
	PSNRResult PSNR;
	PSNRResult I_FRAME_PSNR;
	PSNRResult P_FRAME_PSNR;
};

class CPSNR
{
public:
	CPSNR(PSNRConfig cfg);
	virtual ~CPSNR();
	virtual PSNRResults Compute(avlib::CSequence & seq1, avlib::CSequence & seq2);
	virtual PSNRResultItem<float> Compute(avlib::CImage<uint8_t> & img1, avlib::CImage<uint8_t> & img2);
	virtual float Compute(avlib::CComponent<uint8_t> & cmp1, avlib::CComponent<uint8_t> & cmp2);
protected:
	PSNRConfig m_config;
	PSNRResults m_results;
};

}

#endif //_PSNR_H
