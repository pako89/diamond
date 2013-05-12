#ifndef _ENCODER_H
#define _ENCODER_H


#include <sequence.h>
#include <bitstream.h>
#include <mtimer.h>
#include <utils.h>

namespace avlib
{

class CEncoder : public utils::ITimer
{
public:
	CEncoder();
	virtual ~CEncoder();
	virtual bool Encode(CSequence * pSeq, CBitstream * pBstr) = 0;
protected:
};

}

#endif //_ENCODER_H
