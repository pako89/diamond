#ifndef _CL_PLATFORM_POLICY_H
#define _CL_PLATFORM_POLICY_H

#include <cl_device.h>

class CCLDevicePolicy
{
public:
	CCLDevicePolicy(){}
	virtual ~CCLDevicePolicy(){}
	virtual CCLDevice getDevice() = 0;
protected:
};

class CCLFirstDevicePolicy : public CCLDevicePolicy
{
public:
	CCLFirstDevicePolicy(cl_device_type type = CL_DEVICE_TYPE_GPU);
	CCLDevice getDevice();
protected:		
	virtual bool checkDeviceInfo(CCLDeviceInfo * deviceInfo);
	cl_device_type m_type;
};

#endif //_CL_PLATFORM_POLICY
