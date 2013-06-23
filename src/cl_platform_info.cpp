#include <cl_platform_info.h>


template <class T>
T * getInfo_typeArray(cl_platform_id id,cl_platform_info info,size_t * num)
{
	cl_int err;
	size_t size;
	err = clGetPlatformInfo(id,info,0,NULL,&size);
	if(CL_SUCCESS != err || 0 == size)
	{
		return NULL;
	}
	*num = size/sizeof(T);
	T * ptr = new T[(*num)];
	if(NULL == ptr)
	{
		return NULL;
	}
	err = clGetPlatformInfo(id,info,size,ptr,NULL);
	if(CL_SUCCESS != err)
	{
		delete [] ptr;
		return NULL;
	}
	return ptr;
}

CCLPlatformInfo::CCLPlatformInfo(void)
{
	this->id = 0;
}

CCLPlatformInfo::CCLPlatformInfo(cl_platform_id id)
{
	this->setId(id);
}

CCLPlatformInfo::~CCLPlatformInfo(void)
{
}

cl_platform_id CCLPlatformInfo::getId()
{
	return this->id;
}

void CCLPlatformInfo::setId(cl_platform_id id)
{
	this->id = id;
}


string CCLPlatformInfo::getInfo_string(cl_platform_info info)
{
	size_t num;
	char * c_str = getInfo_typeArray<char>(this->id,info,&num);
	string str(c_str);
	delete [] c_str;
	return str;
}


std::string CCLPlatformInfo::getName()
{
	return this->getInfo_string(CL_PLATFORM_NAME);
}

std::string CCLPlatformInfo::getVendor()
{
	return this->getInfo_string(CL_PLATFORM_VENDOR);
}

std::string CCLPlatformInfo::getVersion()
{
	return this->getInfo_string(CL_PLATFORM_VERSION);
}
std::string CCLPlatformInfo::getProfile()
{
	return this->getInfo_string(CL_PLATFORM_PROFILE);
}
std::string CCLPlatformInfo::getExtensions()
{
	return this->getInfo_string(CL_PLATFORM_EXTENSIONS);
}
