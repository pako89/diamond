#ifndef _CL_HOST_H
#define _CL_HOST_H

#include <cl_device.h>
#include <cl_policy.h>

class ICLHost
{
public:
	ICLHost();
	virtual ~ICLHost();
	void init(CCLDevicePolicy * policy, char * fileName);
protected:
	CCLDevice m_dev;
	cl_program m_program;
	const char * m_fileName;
};

#endif //_CL_HOST_H
