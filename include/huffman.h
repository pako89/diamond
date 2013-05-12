#ifndef _HUFFMAN_H
#define _HUFFMAN_H

#include <bitstream.h>

namespace avlib
{

enum HuffmanItemType
{
	None = 0,
	Leaf = 1,
	Node = 2,
	New = 3,
	EOB = 4,
	ERROR
};

template <class T>
class CHuffman
{
public:
	virtual bool EncodeBlock(T * ptr, uint32_t size, CBitstream * bstr) = 0;
	virtual HuffmanItemType DecodeBlock(T * ptr, uint32_t size, CBitstream * bstr) = 0;
	virtual bool Encode(T symbol,CBitstream * bitstream) = 0;
	virtual HuffmanItemType Decode(CBitstream * bitstream,T * symbol) = 0;
	virtual void Reset() = 0;
protected:

};
}

#endif //_HUFFMAN_H
