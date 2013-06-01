#ifndef _STATIC_RLC_H
#define _STATIC_RLC_H

#include <rlc.h>
#include <utils.h>
#include <static_huffman.h>

namespace avlib
{

template <class T>
class CStaticRLC : public CRLC<T>
{
public:
	CStaticRLC();
	virtual ~CStaticRLC();
	virtual void EncodeBlock(const T * pSrc, CPoint p, CSize s, CBitstream * pBstr);
	virtual void encodeDC(T val, int c, CBitstream * bstr);
	virtual void encodeAC(T val, int run, int c, CBitstream * bstr);
	virtual void Flush(CBitstream * bstr) { m_huff->Flush(bstr); }
	virtual void Reset();
protected:
	CStaticHuffman<T> * m_huff;
};

template <class T>
struct RunValue
{
	RunValue() :
		Run(0),
		Value(0)
	{}
	RunValue(int r, T v) :
		Run(r),
		Value(v)
	{}
	int Run;
	T Value;
};

template <class T>
class CStaticIRLC : public CIRLC<T>
{
public:
public:
	CStaticIRLC();
	virtual ~CStaticIRLC();
	virtual void DecodeBlock(CBitstream * pBstr, T * pDst, CPoint p, CSize s);
	virtual T decodeDC(int c, CBitstream * bstr);
	virtual RunValue<T> decodeAC(int c, CBitstream * bstr);
	virtual void Fill(CBitstream * bstr);
	virtual void Reset();
protected:
	CStaticHuffman<T> * m_huff;
};

}

#endif //_STATIC_RLC_H
