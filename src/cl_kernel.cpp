#include <cl_kernel.h>

namespace avlib
{

ICLKernel::ICLKernel(cl_handle h, cl_program program, const char * kernel) :
	m_h(h),
	m_program(program),
	m_name(kernel)
{
	cl_int err;
	m_kernel = clCreateKernel(m_program, kernel, &err);
	if(CL_SUCCESS != err)
	{
		throw utils::StringFormatException("creating kernel '%s' failed (%d)\n", m_name.c_str(), err);
	}
}

ICLKernel::~ICLKernel()
{
}

}
