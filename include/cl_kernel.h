#ifndef _CL_KERNEL_H
#define _CL_KERNEL_H

#include <utils.h>

#include <cl_device.h>
#include <string>

namespace avlib
{

class ICLKernel
{
public:
	ICLKernel(CCLDevice * dev, cl_program program, const char * kernel);
	virtual ~ICLKernel();
	void EnqueueNDRangeKernel(
			cl_uint work_dim, 
			const size_t *global_work_size, 
			const size_t *local_work_size = NULL,			 	
			cl_uint num_events_in_wait_list = 0,
			const cl_event *event_wait_list = NULL,
			cl_event *event = NULL
			);
	cl_program getProgramId();
	cl_kernel getKernelId();
	std::string getName();
	CCLDevice * getDevice();
protected:
	CCLDevice * m_dev;
	cl_program m_program;
	cl_kernel m_kernel;
	std::string m_name;
	std::vector<size_t> m_max_global_work_size;
	size_t m_max_work_group_size;
};

}

#endif //_CL_KERNEL_H
