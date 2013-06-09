#ifndef _CL_QUANTIZER_H
#define _CL_QUANTIZER_H

#include <utils.h>
#include <avlib.h>
#include <quantizer.h>

#include <cl_kernel.h>
#include <cl_image.h>

namespace avlib
{

class CCLQuant : public CQuant, public ICLKernel
{
public:
	CCLQuant(CCLDevice * dev, cl_program program, const char * kernel);
	virtual void setTables(const uint8_t * YQ, const uint8_t * UQ, const uint8_t * VQ, int qp);
protected:
	virtual void doTransform(CImage<float> * src, CImage<float> * dst);
};

class CCLIQuant : public CIQuant, public ICLKernel
{
public:
	CCLIQuant(CCLDevice * dev, cl_program program, const char * kernel);
	virtual void setTables(const uint8_t * YQ, const uint8_t * UQ, const uint8_t * VQ, int qp);
protected:
	virtual void doTransform(CImage<float> * src, CImage<float> * dst);
};

}

#endif //_CL_QUANTIZER
