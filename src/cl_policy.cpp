#include <cl_policy.h>
#include <cl_base.h>
#include <utils.h>

module("OpenCL::Policy");

CCLDevice CCLFirstDevicePolicy::getDevice()
{
	CCLBase & clBase = CCLBase::getInstance();
	dbg("number of platforms: %d\n", clBase.getPlatformsCount());
	for(int i=0;i<clBase.getPlatformsCount();i++)
	{
		CCLPlatform * p = clBase.getPlatform(i);
#ifdef DEBUG
		CCLPlatformInfo * pi = p->getInfo();
		dbg("platform name: %s\n", pi->getName().c_str());
		dbg("vendor name  : %s\n", pi->getVendor().c_str());
		dbg("version      : %s\n", pi->getVersion().c_str());
#endif 		
		if(NULL != p)
		{
			for(int j=0;j<p->getDevicesCount();j++)
			{
				CCLDevice d = p->getDevice(i);
				if(d.isValid())
				{
					CCLDeviceInfo * i = d.getInfo();
					dbg("checking device: %s [", i->getName().c_str());
					if(this->checkDeviceInfo(i))
					{
						_LOG("Success]\n");
						return d;
					}
#if DEBUG					
					else
					{
						_LOG("Fail]\n");
					}
#endif					
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
