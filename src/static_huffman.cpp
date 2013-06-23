#include <static_huffman.h>

namespace avlib
{

#define CHECK(m_items, dc, c)							\
	items_t::iterator itr = m_items.find(items_key_t(dc, c));		\
	if(itr == m_items.end())						\
	{									\
		throw utils::StringFormatException(				\
				"wrong huffman component (dc=%s, %d)\n", 	\
				dc?"true":"false", 				\
				c						\
				); 						\
	}

#define CHECK_VAL(m_items, val, dc, c)						\
	items_t::iterator itr = m_items.find(items_key_t(dc, c));		\
	if(itr != m_items.end())						\
	{									\
		if(val < 0 || val >= itr->second->Code.size())			\
		{								\
			throw utils::StringFormatException(			\
					"wrong huffman code dc[%d] (0x%02x)\n", \
					c, 					\
					val					\
					);					\
		}								\
	}									\
	else 									\
	{ 									\
		throw utils::StringFormatException(				\
				"wrong huffman component (dc=%s, %d)\n", 	\
				dc?"true":"false", 				\
				c						\
				); 						\
	}									


template <class T>
const uint8_t CStaticHuffman<T>::DC_V[] = {
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 
};

#ifdef WIN32
#define DC_V_SIZE 12
#else
#define DC_V_SIZE ARRAY_SIZE(DC_V)
#endif

template <class T>
const uint8_t CStaticHuffman<T>::DC_Y_L[] = {
 0, 0, 1, 5, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 
};

#ifdef WIN32
#define DC_Y_L_SIZE 17
#else
#define DC_Y_L_SIZE ARRAY_SIZE(DC_Y_L)
#endif

template <class T>
const uint8_t CStaticHuffman<T>::DC_UV_L[] = {
 0, 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 
};

#ifdef WIN32
#define DC_UV_L_SIZE 17
#else
#define DC_UV_L_SIZE ARRAY_SIZE(DC_UV_L)
#endif

template <class T>
const uint8_t CStaticHuffman<T>::AC_Y_V[] = 
{
	0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12,
	0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07,
	0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08,
	0x23, 0x42, 0xb1, 0xc1, 0x15, 0x52, 0xd1, 0xf0,
	0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0a, 0x16,
	0x17, 0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28,
	0x29, 0x2a, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
	0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
	0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
	0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
	0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
	0x7a, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
	0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98,
	0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
	0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6,
	0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5,
	0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4,
	0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2,
	0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea,
	0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
	0xf9, 0xfa
};

#ifdef WIN32
#define AC_Y_V_SIZE 162
#else
#define AC_Y_V_SIZE ARRAY_SIZE(AC_Y_V)
#endif

template <class T>
const uint8_t CStaticHuffman<T>::AC_Y_L[] ={
 0, 0, 2, 1, 3, 3, 2, 4, 3, 5, 5, 4, 4, 0, 0, 1, 0x7d 
};

#ifdef WIN32
#define AC_Y_L_SIZE 17
#else 
#define AC_Y_L_SIZE ARRAY_SIZE(AC_Y_L)
#endif

template <class T>
const uint8_t CStaticHuffman<T>::AC_UV_L[] = {
 0, 0, 2, 1, 2, 4, 4, 3, 4, 7, 5, 4, 4, 0, 1, 2, 0x77
};

#ifdef WIN32
#define AC_UV_L_SIZE 17
#else
#define AC_UV_L_SIZE ARRAY_SIZE(AC_UV_L)
#endif

template <class T>
const uint8_t CStaticHuffman<T>::AC_UV_V[] = {
	0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21,
	0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71,
	0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91,
	0xa1, 0xb1, 0xc1, 0x09, 0x23, 0x33, 0x52, 0xf0,
	0x15, 0x62, 0x72, 0xd1, 0x0a, 0x16, 0x24, 0x34,
	0xe1, 0x25, 0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26,
	0x27, 0x28, 0x29, 0x2a, 0x35, 0x36, 0x37, 0x38,
	0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
	0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
	0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
	0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
	0x79, 0x7a, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
	0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96,
	0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5,
	0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4,
	0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3,
	0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2,
	0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda,
	0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9,
	0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
	0xf9, 0xfa
};

#ifdef WIN32
#define AC_UV_V_SIZE 162
#else
#define AC_UV_V_SIZE ARRAY_SIZE(AC_UV_V)
#endif

template <class T>
CStaticHuffman<T>::CStaticHuffman()
{
	Reset();
	createItem(&m_DCY, CStaticHuffman::DC_Y_L, DC_Y_L_SIZE, CStaticHuffman::DC_V);
	createItem(&m_DCUV, CStaticHuffman::DC_UV_L, DC_UV_L_SIZE, CStaticHuffman::DC_V);
	createItem(&m_ACY, CStaticHuffman::AC_Y_L, AC_Y_L_SIZE, CStaticHuffman::AC_Y_V);
	createItem(&m_ACUV, CStaticHuffman::AC_UV_L, AC_UV_L_SIZE, CStaticHuffman::AC_UV_V);
	m_items[items_key_t(true, 0)] = &m_DCY;
	m_items[items_key_t(true, 1)] = &m_DCUV;
	m_items[items_key_t(true, 2)] = &m_DCUV;
	m_items[items_key_t(false, 0)] = &m_ACY;
	m_items[items_key_t(false, 1)] = &m_ACUV;
	m_items[items_key_t(false, 2)] = &m_ACUV;
}

template <class T>
CStaticHuffman<T>::~CStaticHuffman()
{
}

template <class T>
int CStaticHuffman<T>::getNumberOfCodes(const uint8_t * len, int size, const uint8_t * val)
{
	int ret = 0;
	int k=0;
	for(int i = 1; i < size; i++)
	{
		int nb = len[i];
		for(int j = 0 ; j < nb; j++)
		{
			int sym = val[k++];
			if(sym > ret)
			{
				ret = sym;
			}
		}
	}
	return ret+1;
}

template <class T>
void CStaticHuffman<T>::createItem(HuffmanItem * item, const uint8_t * len, int size, const uint8_t * val)
{
	int s = getNumberOfCodes(len, size, val);
	item->Code.resize(s, 0);
	item->Size.resize(s, 0);
	uint32_t code = 0;
	int k = 0;
	for(int i = 1 ; i < size ; i++)
	{
		int nb = len[i];
		for(int j = 0 ; j < nb; j++)
		{
			uint8_t sym = val[k++];
			item->Size[sym] = i;
	       		item->Code[sym] = code;
			item->Map.insert(std::pair<HuffmanItem::map_key_t, uint8_t>(HuffmanItem::map_key_t(code, i), sym));
		 	code++;
		}
		code <<= 1;
	}
}

template <class T>
void CStaticHuffman<T>::Put(uint32_t bits, int n, CBitstream * bstr)
{
	uint64_t buff = ((uint64_t)m_buff<<n) | (bits&((1<<n)-1));
	m_buff_cnt += n;
	while(m_buff_cnt >= 8)
	{
		uint8_t temp = 0xff&(buff>>(m_buff_cnt-8));
		m_buff_cnt -= 8;
		bstr->putBits(8, temp);
		if(temp == 0xff)
		{
			bstr->putBits(8, 0x00);
		}
	}
	m_buff = (uint32_t)buff;
}

template <class T>
void CStaticHuffman<T>::Flush(CBitstream * bstr)
{
	if(m_buff_cnt)
	{
		bstr->putBits(m_buff_cnt, m_buff);
	}
	m_buff_cnt = 0;
}

template <class T>
void CStaticHuffman<T>::Fill(CBitstream * bstr)
{
	int pos = bstr->getPosition();
	pos -= m_buff_cnt;
	bstr->setPosition(pos);
	m_buff_cnt=0;
}

template <class T>
void CStaticHuffman<T>::feed(CBitstream * bstr)
{
	int n = 8;
	uint32_t b = bstr->getBits(n);
	m_buff = ((m_buff<<n)|(b&((1<<n)-1)));
	m_buff_cnt += 8;
	m_buff = m_buff&((1<<m_buff_cnt)-1);
	if((m_buff&0xff) == 0xff)
	{
		if(bstr->getBits(8) != 0x00)
		{
			throw utils::StringFormatException("Sync");
		}
	}
}

template <class T>
uint32_t CStaticHuffman<T>::Get(int n, CBitstream * bstr)
{
	int tn = n;
	uint32_t mask = (1<<n)-1;
	uint32_t ret=0;
	while(tn > 0)
	{
		if(m_buff_cnt < 8) feed(bstr);
		int _n = m_buff_cnt > tn ? tn : m_buff_cnt;
		ret = (ret << _n) | ((m_buff>>(m_buff_cnt-_n)));
		tn -= _n;
		m_buff_cnt -= _n;
	}
	return ret&mask;
}

template <class T>
huffman_code_t CStaticHuffman<T>::GetCode(CBitstream * bstr, HuffmanItem * item)
{
	std::map<std::pair<uint32_t, int>, uint8_t>::iterator itr;
	uint32_t code = 0;
	int len=0;
	do
	{
		len++;
		code = (code<<1) | Get(1, bstr);
		itr = item->Map.find(HuffmanItem::map_key_t(code, len));
	}while(itr == item->Map.end());
	return itr->second;
}

template <class T>
void CStaticHuffman<T>::Encode(huffman_code_t val, CBitstream * bstr, bool dc, int c)
{
#ifdef CHECK_HUFFMAN
	CHECK_VAL(m_items, val, dc, c);
#endif
	HuffmanItem * item = m_items[items_key_t(dc, c)];
	Put(item->Code[val], item->Size[val], bstr);
}

template <class T>
bool CStaticHuffman<T>::Contains(huffman_code_t val, bool dc, int c)
{
	HuffmanItem * item = m_items[items_key_t(dc, c)];
	return (item->Size.size() > val && item->Size[val] > 0);
}

template <class T>
huffman_code_t CStaticHuffman<T>::Decode(CBitstream * bstr, bool dc, int c)
{
#ifdef CHECK_HUFFMAN
	CHECK(m_items, dc, c);
#endif
	HuffmanItem * item = m_items[items_key_t(dc, c)];
	return GetCode(bstr, item);
}

template <class T>
void CStaticHuffman<T>::Reset()
{
	m_buff = 0;
	m_buff_cnt = 0;
}

INSTANTIATE(CStaticHuffman, uint8_t);
INSTANTIATE(CStaticHuffman, uint16_t);
INSTANTIATE(CStaticHuffman, int16_t);
INSTANTIATE(CStaticHuffman, uint32_t);
INSTANTIATE(CStaticHuffman, int32_t);
INSTANTIATE(CStaticHuffman, float);

}
