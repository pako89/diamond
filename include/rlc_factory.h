#ifndef _RLC_FACTORY_H
#define _RLC_FACTORY_H

#include <rlc.h>
#include <avlib.h>

namespace avlib
{

template <class T>
class CRLCFactory
{
public:
	static CRLC<T> * CreateRLC(HUFFMAN_TYPE type);
};

}

#endif //_RLC_FACTORY_H
