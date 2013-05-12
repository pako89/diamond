#ifndef _DYNAMIC_RLC_H
#define _DYNAMIC_RLC_H

#include <rlc.h>

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
class CDynamicRLC : public CRLC<T>
{
public:
	CDynamicRLC();
	virtual ~CDynamicRLC();
	virtual void EncodeBlock(T * pSrc, CPoint p, CSize s, CBitstream * pBstr);
protected:
	CHuffmanQuad<T> m_quads[3];
};

template <class T>
class CDynamicIRLC : public CIRLC<T>
{
public:
	CDynamicIRLC();
	virtual ~CDynamicIRLC();
	virtual void DecodeBlock(CBitstream * pBstr, T * pDst, CPoint p, CSize s);
protected:
	CHuffmanQuad<T> m_quads[3];	
};

}

#endif //_DYNAMIC_RLC_H
