#include <rlc.h>
#include <utils.h>

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
CRLC<T>::CRLC()
{
	m_quads[0].DCValue = new CHuffmanTree<T>();
	m_quads[0].ACRun = new CHuffmanTree<uint8_t>();
	m_quads[0].ACValue = new CHuffmanTree<T>();
	m_quads[2].ACRun = m_quads[1].ACRun = new CHuffmanTree<uint8_t>();
	m_quads[2].DCValue = m_quads[1].DCValue = new CHuffmanTree<T>();
	m_quads[2].ACValue = m_quads[1].ACValue = new CHuffmanTree<T>();
}

template <class T>
CRLC<T>::~CRLC()
{
	delete m_quads[0].DCValue;
	delete m_quads[0].ACRun;
	delete m_quads[0].ACValue;
	delete m_quads[1].DCValue;
	delete m_quads[1].ACRun;
	delete m_quads[1].ACValue;
}

template <class T>
void CRLC<T>::Encode(CImage<T> * pImg, CBitstream * pBstr)
{
	for(int k=0;k<pImg->getComponents(); k++)
	{
		CSize size((*pImg)[k].getHeight(), (*pImg)[k].getWidth());
		for(int y=0; y < (*pImg)[k].getHeight(); y+=8)
		{
			for(int x=0; x < (*pImg)[k].getWidth(); x+=8)
			{
				EncodeBlock(&(*pImg)[k][0][0], CPoint(k, y, x), size, pBstr);
			}
		}
		pBstr->flush();
	}
	
}

template <class T>
void CRLC<T>::EncodeBlock(T * pSrc, CPoint p, CSize s, CBitstream * pBstr)
{
#ifdef PASS_THROUGH
	T * src = &pSrc[p.Y*s.Width + p.X];
	for(int y=0;y<8;y++)
	{
		for(int x = 0; x<8; x++)
		{
			if(y == 0 && x == 0)
			{
				m_quads[p.Z].DCValue->Encode(src[y*s.Width+x], pBstr);
			}
			else
			{
				m_quads[p.Z].ACValue->Encode(src[y*s.Width+x], pBstr);
			}
		}
	}
#else
	T * src = &pSrc[p.Y*s.Width + p.X];
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

#endif
}

template <class T>
CIRLC<T>::CIRLC()
{
	m_quads[0].DCValue = new CHuffmanTree<T>();
	m_quads[0].ACRun = new CHuffmanTree<uint8_t>();
	m_quads[0].ACValue = new CHuffmanTree<T>();
	m_quads[2].DCValue = m_quads[1].DCValue = new CHuffmanTree<T>();
	m_quads[2].ACRun = m_quads[1].ACRun = new CHuffmanTree<uint8_t>();
	m_quads[2].ACValue = m_quads[1].ACValue = new CHuffmanTree<T>();
}

template <class T>
CIRLC<T>::~CIRLC()
{
	delete m_quads[0].DCValue;
	delete m_quads[0].ACRun;
	delete m_quads[0].ACValue;
	delete m_quads[1].DCValue;
	delete m_quads[1].ACRun;
	delete m_quads[1].ACValue;
}

template <class T>
void CIRLC<T>::Decode(CBitstream * pBstr, CImage<T> * pImg)
{
	for(int k=0;k<pImg->getComponents(); k++)
	{
		CSize size((*pImg)[k].getHeight(), (*pImg)[k].getWidth());
		for(int y=0; y < (*pImg)[k].getHeight(); y+=8)
		{
			for(int x=0; x < (*pImg)[k].getWidth(); x+=8)
			{
				DecodeBlock(pBstr, &(*pImg)[k][0][0], CPoint(k, y, x), size);
			}
		}
	}
}

template <class T>
void CIRLC<T>::DecodeBlock(CBitstream * pBstr, T * pDst, CPoint p, CSize s)
{
#ifdef PASS_THROUGH
	T * dst = &pDst[p.Y*s.Width + p.X];
	for(int y=0;y<8;y++)
	{
		for(int x = 0; x<8; x++)
		{
			if(y == 0 && x == 0)
			{
				m_quads[p.Z].DCValue->Decode(pBstr, &dst[y*s.Width+x]);
			}
			else
			{
				m_quads[p.Z].ACValue->Decode(pBstr, &dst[y*s.Width+x]);
			}
		}
	}
#else
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
		if(run>63)
			printf("run=%d\n", run);
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
#endif
}

INSTANTIATE(CHuffmanQuad, int32_t);
INSTANTIATE(CRLC, int32_t);
INSTANTIATE(CIRLC, int32_t);
INSTANTIATE(CHuffmanQuad, int16_t);
INSTANTIATE(CRLC, int16_t);
INSTANTIATE(CIRLC, int16_t);

}
