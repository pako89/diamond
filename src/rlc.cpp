#include <rlc.h>
#include <utils.h>

namespace avlib
{

template <class T>
CRLC<T>::CRLC()
{
	m_predHuff = new CDynamicHuffman<int32_t>();
}

template <class T>
CRLC<T>::~CRLC()
{
	delete m_predHuff;
}

template <class T>
void CRLC<T>::Encode(CImage<T> * pImg, CBitstream * pBstr)
{
	m_timer.start();
	doEncode(pImg, pBstr);
	m_timer.stop();
}

template <class T>
void CRLC<T>::Encode(CImage<T> * pImg, CPredictionInfoTable * pPred, CBitstream * pBstr)
{
	m_timer.start();
	doEncode(pImg, pPred, pBstr);
	m_timer.stop();
}

template <class T>
void CRLC<T>::doEncode(CImage<T> * pImg, CBitstream * pBstr)
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
	}
}

template <class T>
void CRLC<T>::doEncode(CImage<T> * pImg, CPredictionInfoTable * pPred, CBitstream * pBstr)
{
	for(int y=0;y<pPred->getSize().Height;y++)
	{
		for(int x=0;x<pPred->getSize().Width;x++)
		{
			(*pPred)[y][x].Encode(m_predHuff, pBstr);
		}
	}
	doEncode(pImg, pBstr);
}

template <class T>
CIRLC<T>::CIRLC()
{
	m_predHuff = new CDynamicHuffman<int32_t>();
}

template <class T>
CIRLC<T>::~CIRLC()
{
	delete m_predHuff;
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
void CIRLC<T>::Decode(CBitstream * pBstr, CImage<T> * pImg, CPredictionInfoTable * pPred)
{
	for(int y=0;y<pPred->getSize().Height;y++)
	{
		for(int x=0;x<pPred->getSize().Width;x++)
		{
			(*pPred)[y][x].Decode(m_predHuff, pBstr);
		}
	}
	Decode(pBstr, pImg);
}

INSTANTIATE(CRLC, int32_t);
INSTANTIATE(CIRLC, int32_t);
INSTANTIATE(CRLC, int16_t);
INSTANTIATE(CIRLC, int16_t);

}
