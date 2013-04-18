#ifndef _CL_COMMON_H
#define _CL_COMMON_H

#include <CL/opencl.h>

struct cl_handle
{
	cl_handle() :
		platform_id(0),
		device_id(0),
		context(0),
		queue(0)
	{}
	cl_platform_id platform_id;
	cl_device_id device_id;
	cl_context context;
	cl_command_queue queue;
};

#endif //_CL_COMMON_H
