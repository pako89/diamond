#ifndef _CL_ZIGZAG_H
#define _CL_ZIGZAG_H

#include <zigzag.h>
#include <cl_kernel.h>
#include <cl_image.h>

namespace avlib
{

template <class S, class D>
class CCLZigZag : public CZigZag<S, D>, public ICLKernel
{
public:
	CCLZigZag(CCLDevice * dev, cl_program program, const char * kernel);
protected:
	virtual void doTransform(CImage<S> * src, CImage<D> * dst);
};

}

#endif //_CL_ZIGZAG_H
