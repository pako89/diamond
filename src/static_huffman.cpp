#include <static_huffman.h>

namespace avlib
{

template <class T>
CStaticHuffman<T>::CStaticHuffman()
{
}


template <class T>
CStaticHuffman<T>::~CStaticHuffman()
{
}

template <class T>
bool CStaticHuffman<T>::EncodeBlock(T * ptr, uint32_t size, CBitstream * bstr)
{
}

template <class T>
HuffmanItemType CStaticHuffman<T>::DecodeBlock(T * ptr, uint32_t size, CBitstream * bstr)
{
}

template <class T>
bool CStaticHuffman<T>::Encode(T symbol,CBitstream * bitstream)
{
}

template <class T>
HuffmanItemType CStaticHuffman<T>::Decode(CBitstream * bitstream,T * symbol)
{
}

template <class T>
void CStaticHuffman<T>::Reset()
{
}

INSTANTIATE(CStaticHuffman, uint8_t);
INSTANTIATE(CStaticHuffman, uint16_t);
INSTANTIATE(CStaticHuffman, int16_t);
INSTANTIATE(CStaticHuffman, uint32_t);
INSTANTIATE(CStaticHuffman, int32_t);
INSTANTIATE(CStaticHuffman, float);

}
