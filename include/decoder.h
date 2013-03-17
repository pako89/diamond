#ifndef _DECODER_H
#define _DECODER_H

#include <bitstream.h>
#include <sequence.h>

namespace avlib
{

class CDecoder
{
public:
	CDecoder();
	virtual ~CDecoder();
	virtual bool Decode(CBitstream * pBstr, CSequence * pSeq) = 0;
protected:
};

}

#endif //_DECODER_H
