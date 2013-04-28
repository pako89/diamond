#ifndef _CL_PLATFORM_INFO_H
#define _CL_PLATFORM_INFO_H

#include <CL/opencl.h>
#include <string>

using namespace std;

class CCLPlatformInfo
{
public:
	CCLPlatformInfo();
	CCLPlatformInfo(cl_platform_id id);
	~CCLPlatformInfo(void);
	cl_platform_id getId();
	void setId(cl_platform_id id);
	string getName();
	string getVendor();
	string getVersion();
	string getProfile();
	string getExtensions();
protected:
	string getInfo_string(cl_platform_info info);
	cl_platform_id id;
};

#endif //_CL_PLATFORM_INFO_H
