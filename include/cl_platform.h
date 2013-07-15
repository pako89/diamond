#ifndef _CL_PLATFORM_H
#define _CL_PLATFORM_H

#include <CL/opencl.h>
#include <cl_platform_info.h>
#include <cl_device.h>

class CCLDevice;

class CCLPlatform
{
public:
	CCLPlatform(void);
	~CCLPlatform(void);
	cl_platform_id getID();
	void setID(cl_platform_id id);
	void fillDevices(cl_device_type type = CL_DEVICE_TYPE_ALL);
	cl_uint getDevicesCount();
	CCLDevice getDevice(cl_uint i);
	CCLDevice * getDevicePtr(cl_uint i);
	CCLPlatformInfo * getInfo();
protected:
	CCLPlatformInfo * info;
	cl_platform_id id;
	CCLDevice ** devices;
	cl_uint num_devices;
	
};

#endif //_CL_PLATFORM_H
