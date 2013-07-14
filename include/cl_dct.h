#ifndef _CL_DCT_H
#define _CL_DCT_H

#include <dct.h>

#include <cl_device.h>
#include <cl_image.h>
#include <cl_kernel.h>

namespace avlib
{

class CCLDCT : public CDCT, public ICLKernel
{
public:
	CCLDCT(CCLDevice * dev, cl_program program, const char * kernel);
	virtual ~CCLDCT();
protected:
	virtual void doTransform(CImage<float> * src, CImage<float> * dst);
};

class CCLIDCT : public CCLDCT, public CIDCT
{
public:
	CCLIDCT(CCLDevice * dev, cl_program program, const char * kernel);
	virtual void Transform(CImage<float> * src, CImage<float> * dst)
	{
		CCLDCT::Transform(src, dst);
	}
	virtual utils::CTimer & getTimer()
	{
		return CCLDCT::getTimer();
	}
protected:
	virtual void doTransform(CImage<float> * src, CImage<float> * dst);
};

}
#endif //_CL_DCT_H
