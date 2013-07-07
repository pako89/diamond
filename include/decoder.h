#ifndef _DECODER_H
#define _DECODER_H

#include <bitstream.h>
#include <sequence.h>

namespace avlib
{

class CDecoder
{
public:
	CDecoder(bool printProgressBar = true);
	virtual ~CDecoder();
	virtual bool Decode(CBitstream * pBstr, CSequence * pSeq) = 0;
protected:
	virtual void printProgressBar(int i, int n);
	bool m_progressBar;
};

}

#endif //_DECODER_H
