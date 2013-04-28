#include <cl_kernel.h>

module("OpenCL::Kernel");

namespace avlib
{

ICLKernel::ICLKernel(CCLDevice * dev, cl_program program, const char * kernel) :
	m_dev(dev),
	m_program(program),
	m_name(kernel)
{
	cl_int err;
	m_kernel = clCreateKernel(m_program, kernel, &err);
	if(CL_SUCCESS != err)
	{
		throw utils::StringFormatException("creating kernel '%s' failed (%d)\n", m_name.c_str(), err);
	}
	m_max_global_work_size = m_dev->getInfo()->getMaxWorkItemSizes();
	m_max_work_group_size = m_dev->getInfo()->getMaxWorkGroupSize();
#ifdef DEBUG
	for(int i=0;i<m_max_global_work_size.size();i++)
	{
		dbgf("Max global work size [%d]: %d\n", i, m_max_global_work_size[i]);
	}
	dbgf("Max work group size: %d\n", m_max_work_group_size);
#endif
}

void ICLKernel::EnqueueNDRangeKernel(
			cl_uint work_dim, 
			const size_t *global_work_size, 
			const size_t *local_work_size,
			cl_uint num_events_in_wait_list,
			const cl_event *event_wait_list,
			cl_event *event
			)
{
	cl_int err;
	size_t g_work_size[3];
	size_t global_work_offset[3] = {0,};
	do
	{
		for(int i=0;i<work_dim;i++)
		{
			g_work_size[i] = utils::min(global_work_size[i]-global_work_offset[i], m_max_global_work_size[i]);
			dbgf("%s: global_work_size[%d] = %d\n", m_name.c_str(), i, global_work_size[i]);
			dbgf("%s: local_work_size[%d] = %d\n", m_name.c_str(), i, local_work_size[i]);
			dbgf("%s: g_work_size[%d] = %d\n", m_name.c_str(), i, g_work_size[i]);
		}
	}while(false);
	err = clEnqueueNDRangeKernel(
			this->m_dev->getCommandQueue(), 
			m_kernel, 
			work_dim, 
			NULL, 
			global_work_size, 
			local_work_size, 
			num_events_in_wait_list, 
			event_wait_list, 
			event);
	if(CL_SUCCESS != err) throw utils::StringFormatException("clEnqueueNDRangeKernel(%d)\n", err);
}


ICLKernel::~ICLKernel()
{
}

cl_program ICLKernel::getProgramId()
{
	return m_program;
}

cl_kernel ICLKernel::getKernelId()
{
	return m_kernel;
}

std::string ICLKernel::getName()
{
	return m_name;
}

CCLDevice * ICLKernel::getDevice()
{
	return m_dev;
}

}
