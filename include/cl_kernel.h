#ifndef _CL_KERNEL_H
#define _CL_KERNEL_H

#include <utils.h>
#include <cl_common.h>
#include <string>

namespace avlib
{

class ICLKernel
{
public:
	ICLKernel(cl_handle h, cl_program program, const char * kernel);
	virtual ~ICLKernel();
protected:
	cl_handle m_h;
	cl_program m_program;
	cl_kernel m_kernel;
	std::string m_name;
};

}

#endif //_CL_KERNEL_H
