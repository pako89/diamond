#ifndef _CL_DEVICE_H
#define _CL_DEVICE_H

#include <CL/opencl.h>
#include <string>
#include <utils.h>

#include <cl_device_info.h>
#include <cl_platform.h>

class CCLPlatform;

class CCLDevice
{
public:
	CCLDevice(void);
	CCLDevice(const CCLDevice & src);
	CCLDevice(CCLPlatform * platform,cl_device_id device_id);
	~CCLDevice(void);
	bool operator==(CCLDevice & src);
	bool isValid();
	CCLDeviceInfo * getInfo();
	cl_platform_id getPlatformId();
	CCLPlatform * getPlatform();
	cl_device_id getId();
	void setId(CCLPlatform * platform, cl_device_id device_id);
	void createContext();
	cl_context getContext();
	void createCommandQueue();
	cl_command_queue getCommandQueue();
	void Finish();
protected:
	CCLPlatform * m_platform;
	cl_device_id id;
	CCLDeviceInfo * info;
	cl_context m_context;
	cl_command_queue m_queue;
};

#endif //_CL_DEVICE_H
