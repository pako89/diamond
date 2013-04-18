#ifndef _CL_DCT_H
#define _CL_DCT_H

#include <dct.h>
#include <cl_common.h>
#include <cl_image.h>
#include <cl_dct.h>

namespace avlib
{

class CCLDCT : public CDCT
{
public:
	CCLDCT(cl_handle h, cl_program program, const char * kernel);
	virtual ~CCLDCT();
	virtual void Transform(CImage<float> * src, CImage<float> * dst);
protected:
	cl_handle m_h;
	cl_program m_program;
	cl_kernel m_kernel;
};

}

#endif //_CL_DCT_H
