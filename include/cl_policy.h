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
	CCLFirstDevicePolicy(){}
	CCLDevice getDevice();
protected:		
	virtual bool checkDeviceInfo(CCLDeviceInfo * deviceInfo);
};

class CCLFirstGPUDevicePolicy : public CCLFirstDevicePolicy
{
public:
	CCLFirstGPUDevicePolicy(){}
protected:
	virtual bool checkDeviceInfo(CCLDeviceInfo * deviceInfo);
};

#endif //_CL_PLATFORM_POLICY
