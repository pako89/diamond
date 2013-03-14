#ifndef _HUFFMAN_TREE_H
#define _HUFFMAN_TREE_H

#include <bitstream.h>
#include <stdio.h>
#include <stdint.h>
#include <utils.h>

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
class CHuffmanTree
{
public:
	CHuffmanTree();
	CHuffmanTree(const CHuffmanTree & source);
	~CHuffmanTree(void);
	bool EncodeBlock(void * ptr,uint32_t size,CBitstream * bstr= NULL,bool inc_new = true);
	HuffmanItemType DecodeBlock(void * ptr,uint32_t size,CBitstream * bstr= NULL);
	bool Encode(T symbol,CBitstream * bitstream = NULL,bool inc_new = true);
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
	void Copy(const CHuffmanTree & source);
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

friend class CHuffmanTree<T>;
};




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}
#endif //_HUFFMAN_TREE_H