#include <cl_base.h>

CCLBase * CCLBase::m_instance = NULL;

CCLBase & CCLBase::getInstance()
{
	if(NULL == CCLBase::m_instance)
	{
		CCLBase::m_instance = new CCLBase();
	}
	return *CCLBase::m_instance;
}

void CCLBase::releaseInstance()
{
	if(NULL != CCLBase::m_instance)
	{
		delete CCLBase::m_instance;
	}
}

CCLBase::CCLBase(void)
{
	this->platforms = NULL;
	this->num_platforms = 0;
	fillPlatforms();
	fillDevices();
}

CCLBase::~CCLBase(void)
{
}

cl_uint CCLBase::getPlatformsCount()
{
	return this->num_platforms;
}

CCLPlatform* CCLBase::getPlatform(cl_uint i)
{
	if(i < this->num_platforms)
	{
		return &this->platforms[i];
	}
	return NULL;
}

void CCLBase::fillPlatforms()
{
	cl_platform_id * platforms = NULL;
	cl_uint num;
	cl_int err;
	this->num_platforms=0;
	if(NULL != this->platforms)
	{
		delete [] this->platforms;
		this->platforms = NULL;
	}
	err = clGetPlatformIDs(0,NULL,&num);
	if(CL_SUCCESS != err)
	{
		return;
	}
	if(0 == num)
	{
		this->platforms = NULL;
		this->num_platforms = 0;
		return;
	}
	platforms = new cl_platform_id[num];
	if(NULL == platforms)
	{
		return;
	}
	err = clGetPlatformIDs(num,platforms,NULL);
	if(CL_SUCCESS != err)
	{
		return;
	}
	this->platforms = new CCLPlatform[num];
	if(NULL == this->platforms)
	{
		return;
	}
	this->num_platforms = num;
	for(cl_uint i=0;i<num;i++)
	{
		this->platforms[i].setID(platforms[i]);
	}
}

void CCLBase::fillDevices()
{
	for(cl_uint i = 0 ; i < this->num_platforms ; i++)
		this->platforms[i].fillDevices();
}
