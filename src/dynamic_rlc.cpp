#include <dynamic_rlc.h>

namespace avlib
{

template<class T>
CHuffmanQuad<T>::CHuffmanQuad()
{
}

template<class T>
CHuffmanQuad<T>::~CHuffmanQuad()
{
}


template <class T>
CDynamicRLC<T>::CDynamicRLC()
{
	m_quads[0].DCValue = new CDynamicHuffman<T>();
	m_quads[0].ACRun = new CDynamicHuffman<uint8_t>();
	m_quads[0].ACValue = new CDynamicHuffman<T>();
	m_quads[2].DCValue = m_quads[1].DCValue = new CDynamicHuffman<T>();
	m_quads[2].ACRun = m_quads[1].ACRun = new CDynamicHuffman<uint8_t>();
	m_quads[2].ACValue = m_quads[1].ACValue = new CDynamicHuffman<T>();
}

template <class T>
CDynamicRLC<T>::~CDynamicRLC()
{
	delete m_quads[0].DCValue;
	delete m_quads[0].ACRun;
	delete m_quads[0].ACValue;
	delete m_quads[1].DCValue;
	delete m_quads[1].ACRun;
	delete m_quads[1].ACValue;

}

template <class T>
void CDynamicRLC<T>::EncodeBlock(const T * pSrc, CPoint p, CSize s, CBitstream * pBstr)
{
	const T * src = &pSrc[p.Y*s.Width + p.X];
	uint8_t run=0;
	for(int y=0;y<8;y++)
	{
		for(int x = 0; x<8; x++)
		{
			T val = src[y*s.Width+x];
			if(y==0 && x==0)
			{
				m_quads[p.Z].DCValue->Encode(val, pBstr);
			}
			else
			{
				if(val == 0)
				{
					run++;
				}
				else
				{
					m_quads[p.Z].ACRun->Encode(run, pBstr);
					m_quads[p.Z].ACValue->Encode(val, pBstr);
					run = 0;
				}
			}
		}
	}
	if(run)
	{
		m_quads[p.Z].ACRun->Encode(run-1, pBstr);
		m_quads[p.Z].ACValue->Encode(0, pBstr);
	}
}

template <class T>
CDynamicIRLC<T>::CDynamicIRLC()
{
	m_quads[0].DCValue = new CDynamicHuffman<T>();
	m_quads[0].ACRun = new CDynamicHuffman<uint8_t>();
	m_quads[0].ACValue = new CDynamicHuffman<T>();
	m_quads[2].DCValue = m_quads[1].DCValue = new CDynamicHuffman<T>();
	m_quads[2].ACRun = m_quads[1].ACRun = new CDynamicHuffman<uint8_t>();
	m_quads[2].ACValue = m_quads[1].ACValue = new CDynamicHuffman<T>();
}

template <class T>
CDynamicIRLC<T>::~CDynamicIRLC()
{
	delete m_quads[0].DCValue;
	delete m_quads[0].ACRun;
	delete m_quads[0].ACValue;
	delete m_quads[1].DCValue;
	delete m_quads[1].ACRun;
	delete m_quads[1].ACValue;

}

template <class T>
void CDynamicIRLC<T>::DecodeBlock(CBitstream * pBstr, T * pDst, CPoint p, CSize s)
{
	T * dst = &pDst[p.Y*s.Width + p.X];
	m_quads[p.Z].DCValue->Decode(pBstr, &dst[0]);
	int i=1;
	uint8_t run;
	T val;
	int y;
	int x;
	do
	{
		m_quads[p.Z].ACRun->Decode(pBstr, &run);
		m_quads[p.Z].ACValue->Decode(pBstr, &val);
		while(run--)
		{
			y = i/8;
			x = i%8;
			dst[y*s.Width+x] = 0;
			i++;
		}
		y = i/8;
		x = i%8;
		dst[y*s.Width+x] = val;
		i++;
	}while(i<64);
}

INSTANTIATE(CHuffmanQuad, int32_t);
INSTANTIATE(CHuffmanQuad, int16_t);
INSTANTIATE(CDynamicRLC, int16_t);
INSTANTIATE(CDynamicIRLC, int16_t);

}
