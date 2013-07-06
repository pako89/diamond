#ifndef _CL_INTERPOLATION_H
#define _CL_INTERPOLATION_H

#include <interpolation.h>
#include <cl_kernel.h>

namespace avlib
{

template <class T>
class CCLInterpolation : public CInterpolation<T>
{
public:
	CCLInterpolation(CCLDevice * dev, cl_program program, const char * kernel);
	~CCLInterpolation();
protected:
	virtual void doTransform(CImage<T> * src, CImage<T> * dst);
	ICLKernel * m_kernel;
};

}

#endif //_CL_INTERPOLATION_H
