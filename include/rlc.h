#ifndef _RLC_H
#define _RLC_H

#include <dynamic_huffman.h>
#include <image.h>
#include <mtimer.h>

namespace avlib
{

template <class T>
class CRLC : public utils::ITimer
{
public:
	CRLC();
	virtual ~CRLC();
	virtual void Encode(CImage<T> * pImg, CBitstream * pBstr);
	virtual void EncodeBlock(const T * pSrc, CPoint p, CSize s, CBitstream * pBstr) = 0;
protected:
	virtual void doEncode(CImage<T> * pImg, CBitstream * pBstr);
};

template <class T>
class CIRLC
{
public:
	CIRLC();
	virtual ~CIRLC();
	virtual void Decode(CBitstream * pBstr, CImage<T> * pImg);
	virtual void DecodeBlock(CBitstream * pBstr, T * pDst, CPoint p, CSize s) = 0;
protected:
};

}

#endif //_RLC_H
