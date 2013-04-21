#ifndef _CL_DCT_H
#define _CL_DCT_H

#include <dct.h>
#include <cl_common.h>
#include <cl_image.h>
#include <cl_kernel.h>

namespace avlib
{

class CCLDCT : public CDCT, public ICLKernel
{
public:
	CCLDCT(cl_handle h, cl_program program, const char * kernel);
	virtual ~CCLDCT();
	virtual void Transform(CImage<float> * src, CImage<float> * dst);
protected:
};

}
#endif //_CL_DCT_H
