#ifndef _CL_BASE_H
#define _CL_BASE_H

#include <CL/opencl.h>
#include "cl_platform.h"

class CCLBase
{
public:
	static CCLBase & getInstance();
	static void releaseInstance();
protected:
	static CCLBase * m_instance;
public:
	~CCLBase(void);
	cl_uint getPlatformsCount();
	CCLPlatform * getPlatform(cl_uint i);
protected:
	CCLBase(void);
	void fillPlatforms();
	void fillDevices();
	CCLPlatform * platforms;
	cl_uint num_platforms;
};

#endif //_CL_BASE_H
