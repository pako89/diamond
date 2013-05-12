#ifndef _RLC_H
#define _RLC_H

#include <dynamic_huffman.h>
#include <image.h>
#include <mtimer.h>

namespace avlib
{

template<class T>
class CHuffmanQuad
{
public:
	CHuffmanQuad();
	virtual ~CHuffmanQuad();
       	CHuffman<T> * DCValue;
	CHuffman<uint8_t> * ACRun;
       	CHuffman<T> * ACValue;
};

template <class T>
class CRLC : public utils::ITimer
{
public:
	CRLC();
	virtual ~CRLC();
	virtual void Encode(CImage<T> * pImg, CBitstream * pBstr);
	virtual void EncodeBlock(T * pSrc, CPoint p, CSize s, CBitstream * pBstr);
protected:
	CHuffmanQuad<T> m_quads[3];
};

template <class T>
class CIRLC
{
public:
	CIRLC();
	virtual ~CIRLC();
	virtual void Decode(CBitstream * pBstr, CImage<T> * pImg);
	virtual void DecodeBlock(CBitstream * pBstr, T * pDst, CPoint p, CSize s);
protected:
	CHuffmanQuad<T> m_quads[3];
};

}

#endif //_RLC_H
