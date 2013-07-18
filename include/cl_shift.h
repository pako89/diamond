#ifndef _CL_SHIFT_H
#define _CL_SHIFT_H

#include <utils.h>
#include <avlib.h>
#include <shift.h>

#include <cl_kernel.h>
#include <cl_image.h>

namespace avlib
{

template <class T>
class CCLShift : public CShift<T>
{
public:
	CCLShift(CCLDevice * dev, cl_program program, const char * kernel);
	CCLShift(T s, CCLDevice * dev, cl_program program, const char * kernel);
protected:
	virtual void doTransform(CImage<T> * src, CImage<T> * dst);
	CCLKernel * m_kernel;
};

}

#endif //_CL_SHIFT_H
