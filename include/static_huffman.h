#ifndef _STATIC_HUFFMAN_H
#define _STATIC_HUFFMAN_H

#include <huffman.h>
#include <utils.h>

namespace avlib
{

template <class T>
class CStaticHuffman
{
public:
	CStaticHuffman();
	virtual ~CStaticHuffman();
	bool EncodeBlock(T * ptr, uint32_t size, CBitstream * bstr);
	virtual HuffmanItemType DecodeBlock(T * ptr, uint32_t size, CBitstream * bstr);
	virtual bool Encode(T symbol,CBitstream * bitstream);
	virtual HuffmanItemType Decode(CBitstream * bitstream,T * symbol);
	virtual void Reset();
protected:
};

}

#endif //_STATIC_HUFFMAN_H
