#include <cl_policy.h>
#include <cl_base.h>
#include <utils.h>

CCLDevice CCLFirstDevicePolicy::getDevice()
{
	CCLBase & clBase = CCLBase::getInstance();
	for(cl_uint i=0;i<clBase.getPlatformsCount();i++)
	{
		CCLPlatform * p = clBase.getPlatform(i);
		if(NULL != p)
		{
			for(cl_uint j=0;j<p->getDevicesCount();j++)
			{
				CCLDevice d = p->getDevice(i);
				if(d.isValid())
				{
					CCLDeviceInfo * i = d.getInfo();
					if(this->checkDeviceInfo(i))
					{
						return d;
					}
				}
			}
		}
	}
	return CCLDevice();
}

bool CCLFirstDevicePolicy::checkDeviceInfo(CCLDeviceInfo * deviceInfo)
{
	return (NULL != deviceInfo);
}

bool CCLFirstGPUDevicePolicy::checkDeviceInfo(CCLDeviceInfo * deviceInfo)
{
	return CCLFirstDevicePolicy::checkDeviceInfo(deviceInfo) && (deviceInfo->getDeviceType() == CL_DEVICE_TYPE_GPU);
}
