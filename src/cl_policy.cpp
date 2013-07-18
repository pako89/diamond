#include <cl_policy.h>
#include <cl_base.h>
#include <utils.h>

CCLFirstDevicePolicy::CCLFirstDevicePolicy(cl_device_type type) : 
	m_type(type)
{
}

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
				CCLDevice d = p->getDevice(j);
				if(d.isValid())
				{
					CCLDeviceInfo * info = d.getInfo();
					if(this->checkDeviceInfo(info))
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
	return (NULL != deviceInfo) && (deviceInfo->getDeviceType() == m_type);
}
