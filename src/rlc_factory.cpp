#include <rlc_factory.h>
#include <static_rlc.h>
#include <dynamic_rlc.h>

namespace avlib
{

template <class T>
CRLC<T> * CRLCFactory<T>::CreateRLC(HUFFMAN_TYPE type)
{
	switch(type)
	{
	case HUFFMAN_TYPE_STATIC:
		return new CStaticRLC<T>();
	case HUFFMAN_TYPE_DYNAMIC:
		return new CDynamicRLC<T>();
	default:
		return NULL;
	}
}

INSTANTIATE(CRLCFactory, int16_t);

}
