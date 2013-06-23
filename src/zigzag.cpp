#include <zigzag.h>
#include <utils.h>

namespace avlib
{

template <class S, class D>
const uint8_t CZigZag<S, D>::s_lut[64] = {
 0,   1,   5,   6,  14,  15,  27,  28, 
 2,   4,   7,  13,  16,  26,  29,  42, 
 3,   8,  12,  17,  25,  30,  41,  43, 
 9,  11,  18,  24,  31,  40,  44,  53, 
10,  19,  23,  32,  39,  45,  52,  54, 
20,  22,  33,  38,  46,  51,  55,  60, 
21,  34,  37,  47,  50,  56,  59,  61, 
35,  36,  48,  49,  57,  58,  62,  63
};

#define LUT_SIZE	64

template <class S, class D>
const uint8_t CZigZag<S, D>::s_tul[64] = {
 0,  1,  8, 16,  9,  2,  3, 10, 
17, 24, 32, 25, 18, 11,  4,  5,  
12, 19, 26, 33, 40, 48, 41, 34, 
27, 20, 13,  6,  7, 14, 21, 28, 
35, 42, 49, 56, 57, 50, 43, 36, 
29, 22, 15, 23, 30, 37, 44, 51, 
58, 59, 52, 45, 38, 31, 39, 46, 
53, 60, 61, 54, 47, 55, 62, 63, 
};

#define TUL_SIZE		64

template <class S, class D>
CZigZag<S, D>::CZigZag() :
	m_lut(s_lut)
{
}

template <class S, class D>
CZigZag<S, D>::~CZigZag()
{
}

template <class S, class D>
void CZigZag<S, D>::TransformBlock(S * pSrc, D * pDst, CPoint p, CSize s)
{
	S * src = &pSrc[p.Y*s.Width + p.X];
	D * dst = &pDst[p.Y*s.Width + p.X];
	for(int y=0;y<8;y++)
	{
		for(int x=0;x<8;x++)
		{
			CPoint np = getPoint(CPoint(y, x));
			dst[np.Y*s.Width + np.X] = (D)src[y*s.Width+x];
		}
	}
}

template <class S, class D>
CPoint CZigZag<S, D>::getPoint(CPoint p)
{	
	int index = p.Y * 8 + p.X;
	if(index < 0 || index >= LUT_SIZE)
	{
		throw utils::StringFormatException("ZigZag: index too large");
	}
	int new_index = m_lut[index];
	return CPoint(new_index/8, new_index%8);
}

template <class S, class D>
CIZigZag<S, D>::CIZigZag()
{
	CZigZag<S, D>::m_lut = CZigZag<S, D>::s_tul;
}

INSTANTIATE2(CZigZag, float, int16_t);
INSTANTIATE2(CIZigZag, int16_t, float);
INSTANTIATE2(CZigZag, float, int32_t);
INSTANTIATE2(CIZigZag, int32_t, float);
INSTANTIATE2(CIZigZag, int16_t, int16_t);
}
