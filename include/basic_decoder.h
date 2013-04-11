#ifndef _BASIC_DECODER_H
#define _BASIC_DECODER_H

#include <decoder.h>
#include <proto.h>
#include <huffman.h>
#include <dct.h>
#include <quantizer.h>
#include <zigzag.h>
#include <rlc.h>

namespace avlib
{

class CBasicDecoder : public CDecoder
{
public:
	CBasicDecoder();
	virtual ~CBasicDecoder();
	virtual bool Decode(CBitstream * pBstr, CSequence * pSeq);
protected:
};

}

#endif //_BASIC_DECODER_H
