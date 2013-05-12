#include <rlc.h>
#include <utils.h>

namespace avlib
{

template <class T>
CRLC<T>::CRLC()
{
}

template <class T>
CRLC<T>::~CRLC()
{
}

template <class T>
void CRLC<T>::Encode(CImage<T> * pImg, CBitstream * pBstr)
{
	m_timer.start();
	doEncode(pImg, pBstr);
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
		pBstr->flush();
	}
}


template <class T>
CIRLC<T>::CIRLC()
{
}

template <class T>
CIRLC<T>::~CIRLC()
{
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

INSTANTIATE(CRLC, int32_t);
INSTANTIATE(CIRLC, int32_t);
INSTANTIATE(CRLC, int16_t);
INSTANTIATE(CIRLC, int16_t);

}
