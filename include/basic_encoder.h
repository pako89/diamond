#ifndef _BASIC_ENCODER_H
#define _BASIC_ENCODER_H

#include <encoder.h>
#include <proto.h>
#include <huffman.h>
#include <dct.h>
#include <quantizer.h>
#include <zigzag.h>

namespace avlib
{

class CBasicEncoder : public CEncoder
{
public:
	CBasicEncoder();
	virtual ~CBasicEncoder();
	virtual bool Encode(CSequence * pSeq, CBitstream * pBstr);
protected:
};

}

#endif //_BASIC_ENCODER_H
