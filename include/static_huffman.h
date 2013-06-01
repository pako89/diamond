#ifndef _STATIC_HUFFMAN_H
#define _STATIC_HUFFMAN_H

#include <utils.h>
#include <vector>
#include <map>
#include <bitstream.h>

namespace avlib
{

enum HuffmanComponent
{
	DC_Y = 0,
	DC_U,
	DC_V,
	AC_Y,
	AC_U,
	AC_V
};

typedef uint8_t huffman_code_t;

struct HuffmanItem
{
	std::vector<uint32_t> Code;
	std::vector<uint8_t> Size;
	typedef std::pair<uint32_t, int> map_key_t;
	typedef std::map<map_key_t, uint8_t> map_t;
	map_t Map;
};

template <class T>
class CStaticHuffman
{
public:
	static const uint8_t DC_V[];
	static const uint8_t DC_Y_L[];
	static const uint8_t DC_UV_L[];
	static const uint8_t AC_Y_V[];
	static const uint8_t AC_Y_L[];
	static const uint8_t AC_UV_L[];
	static const uint8_t AC_UV_V[];
	
public:
	CStaticHuffman();
	~CStaticHuffman();
	virtual void Encode(huffman_code_t val, CBitstream * bstr, bool dc, int c);
	virtual huffman_code_t Decode(CBitstream * bstr, bool dc, int c);
	virtual void Reset();
	virtual void createItem(HuffmanItem * item, const uint8_t * len, int size, const uint8_t * val);
	virtual void Put(uint32_t bits, int n, CBitstream * bstr);
	virtual uint32_t Get(int n, CBitstream * bstr);
	virtual huffman_code_t GetCode(CBitstream * bstr, HuffmanItem * item);
	virtual void Flush(CBitstream * bstr);
	virtual void Fill(CBitstream * bstr);
	virtual bool Contains(huffman_code_t s, bool dc, int c);
protected:
	virtual int getNumberOfCodes(const uint8_t * len, int size, const uint8_t * val);
	virtual void feed(CBitstream * bstr);
	typedef std::pair<bool, int> items_key_t;
	typedef std::map<items_key_t, HuffmanItem*> items_t;
	items_t m_items;
	uint32_t m_buff;
	int m_buff_cnt;
	HuffmanItem m_DCY;
	HuffmanItem m_ACY;
	HuffmanItem m_DCUV;
	HuffmanItem m_ACUV;
};

}

#endif //_STATIC_HUFFMAN_H
