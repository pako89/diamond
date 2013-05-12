#ifndef _DYNAMIC_HUFFMAN_TREE_H
#define _DYNAMIC_HUFFMAN_TREE_H

#include <bitstream.h>
#include <stdio.h>
#include <stdint.h>
#include <utils.h>
#include <huffman.h>

namespace avlib
{

template <class T> class CHuffmanItem;

class CHuffmanCode
{
public:
	CHuffmanCode();
	~CHuffmanCode();
	uint32_t * Bits;
	int16_t Length;
	int16_t Size;
	uint32_t bits;
	void Put(bool bit);
	void Write(CBitstream * bitstream);

};

template <class T> 
class CDynamicHuffman : public CHuffman<T>
{
public:
	CDynamicHuffman();
	CDynamicHuffman(const CDynamicHuffman & source);
	~CDynamicHuffman(void);
	bool EncodeBlock(T * ptr,uint32_t size,CBitstream * bstr, bool inc_new);
	bool EncodeBlock(T * ptr,uint32_t size,CBitstream * bstr= NULL);
	HuffmanItemType DecodeBlock(T * ptr,uint32_t size,CBitstream * bstr= NULL);
	bool Encode(T symbol,CBitstream * bitstream, bool inc_new);
	bool Encode(T symbol,CBitstream * bitstream = NULL);
	bool EncodeCtl(HuffmanItemType itemtype,CBitstream * bitstream = NULL);
	HuffmanItemType Decode(CBitstream * bitstream,T * symbol);
	void Reset();
	void Leak();
	uint32_t ItemCounter();
	uint32_t SymbolCounter();
	uint64_t Counter();
	void PrintAll();
	void DeleteAll();
	void setSymbolSize(uint8_t ss);
	uint8_t getSymbolSize();
  private:
	uint8_t symbol_size;
	uint32_t leafCounter;
	//void Add(T value,uint32_t weight,HuffmanItemType itemtype);
	CHuffmanItem<T> * First;
	CHuffmanItem<T> * Last;
	CHuffmanCode * GetCode(CHuffmanItem<T> * symbol);
	CHuffmanItem<T> * FindLeafValue(T Value);
	CHuffmanItem<T> * FindItemType(HuffmanItemType itemtype);
	void Init();

	void Insert(T symbol,HuffmanItemType itemtype = Leaf);
	void Copy(const CDynamicHuffman & source);
};

template <class T> 
class CHuffmanItem
{
public:
	~CHuffmanItem();
	CHuffmanItem();
	CHuffmanItem(const CHuffmanItem & source);
  private:
	CHuffmanItem(T value,uint64_t weight,CHuffmanItem * next,CHuffmanItem * parent,CHuffmanItem * left,CHuffmanItem * right,HuffmanItemType itemtype,CHuffmanItem * prev = NULL,uint32_t n = 0);
	void IncWeightAndModify();
	void DecWeight();
	void DecWeightAndModify();
	void Print();
	void Exchange(CHuffmanItem * p,bool update_children = true);
	CHuffmanItem * FindN(uint32_t n);
	T Value;
	uint32_t SN;
	uint64_t Weight;
	CHuffmanItem * Next;
	CHuffmanItem * Prev;	
	CHuffmanItem * Left;		
	CHuffmanItem * Right;	
	CHuffmanItem * Parent;		
	HuffmanItemType ItemType;	

friend class CDynamicHuffman<T>;
};




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}
#endif //_HUFFMAN_TREE_H
