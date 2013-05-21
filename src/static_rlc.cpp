#include <static_rlc.h>

#define CORRECT_MANT(val, mant, nbits)					\
	if(0 == (mant&(1<<(nbits-1))))					\
	{								\
		val = (int16_t)((0xffff&~((1<<nbits)-1))|(mant+1));	\
	}								

namespace avlib
{

template <class T>
void CStaticRLC<T>::Reset()
{
	m_huff->Reset();
}

template <class T>
CStaticRLC<T>::CStaticRLC()
{
	m_huff = new CStaticHuffman<T>();
}

template <class T>
CStaticRLC<T>::~CStaticRLC()
{
	delete m_huff;
}

template <class T>
void CStaticRLC<T>::EncodeBlock(const T * pSrc, CPoint p, CSize s, CBitstream * pBstr)
{
	int run=0;
	int counter=0;
	const T * src = &pSrc[p.Y * s.Width + p.X];
	for(int y = 0 ; y < 8 ; y++)
	{
		for(int x = 0 ; x < 8 ; x++)
		{
			counter++;
			T val = src[y*s.Width + x];
			if(y == 0 && x == 0)
			{
				encodeDC(val, p.Z, pBstr);
			}
			else
			{
				if(val==0)
				{
					run++;
				}
				else
				{
					encodeAC(val, run, p.Z, pBstr);
					run=0;
				}
			}
		}
	}
	if(counter < 63 || run != 0)
	{
		m_huff->Encode(0, pBstr, false, p.Z);
	}
}

template <class T>
void CStaticRLC<T>::encodeDC(T val, int c, CBitstream * bstr)
{
	if(val == 0)
	{
		m_huff->Encode(0, bstr, true, c);
	}
	else
	{
		T mant = val;
		T _val = val;
		if(val < 0)
		{
			val = -val;
			mant--;
		}
		int nbits = utils::log2(val)+1;
		m_huff->Encode(nbits, bstr, true, c);
		m_huff->Put(mant, nbits, bstr);
	}
}

template <class T>
void CStaticRLC<T>::encodeAC(T val, int run, int c, CBitstream * bstr)
{
	while(run >= 16)
	{
		m_huff->Encode(0xf0, bstr, false, c);
		run -= 16;
	}
	T mant = val;
	if(val < 0)
	{
		mant--;
		val = -val;
	}
	int nbits = utils::log2(val) + 1;
	int code = (run<<4)|nbits;
	m_huff->Encode(code, bstr, false, c);
	m_huff->Put(mant, nbits, bstr);
}

template <class T>
void CStaticIRLC<T>::Reset()
{
	m_huff->Reset();
}

template <class T>
CStaticIRLC<T>::CStaticIRLC()
{
	m_huff = new CStaticHuffman<T>();
}

template <class T>
CStaticIRLC<T>::~CStaticIRLC()
{
	delete m_huff;
}

template <class T>
void CStaticIRLC<T>::DecodeBlock(CBitstream * pBstr, T * pDst, CPoint p, CSize s)
{
	T * dst = &pDst[p.Y * s.Width + p.X];
	T val = decodeDC(p.Z, pBstr);
	dst[0] = val;
	int i=1;
	do
	{
		RunValue<T> rv = decodeAC(p.Z, pBstr);
		if(rv.Run == 0 && rv.Value == 0)
		{
			rv.Run = 63-i;
		}
		while(rv.Run--) 
		{
			dst[i/8*s.Width+i%8] = 0;
			i++;
		}
		if(i < 64) 
		{
			dst[i/8*s.Width+i%8] = rv.Value;
			i++;
		}
	}while(i<64);
}

template <class T>
T CStaticIRLC<T>::decodeDC(int c, CBitstream * bstr)
{
	int nbits = m_huff->Decode(bstr, true, c);
	if(nbits)
	{
		T mant = m_huff->Get(nbits, bstr);
		T val = mant;
		CORRECT_MANT(val, mant, nbits);
		return val;
	}
	else
	{
		return 0;
	}
}


template <class T>
RunValue<T> CStaticIRLC<T>::decodeAC(int c, CBitstream * bstr)
{
	int run=0;
	huffman_code_t code;
	while((code = m_huff->Decode(bstr, false, c)) == 0xf0)
	{
		run += 16;
	}
	if(code==0x00)
	{
		return RunValue<T>(0, 0);
	}
	run += (code>>4);
	int nbits = (code&0xf);
	T mant = m_huff->Get(nbits, bstr);
	T val = mant;
	CORRECT_MANT(val, mant, nbits);
	return RunValue<T>(run, val);
}

INSTANTIATE(CStaticRLC, int32_t);
INSTANTIATE(CStaticIRLC, int32_t);
INSTANTIATE(CStaticRLC, int16_t);
INSTANTIATE(CStaticIRLC, int16_t);
}
