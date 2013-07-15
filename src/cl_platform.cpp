#include <cl_platform.h>


CCLPlatform::CCLPlatform(void)
{
	this->id = 0;
	this->devices = NULL;
	this->num_devices = 0;
	this->info = new CCLPlatformInfo();
}


CCLPlatform::~CCLPlatform(void)
{
	if(NULL != this->info)
	{
		delete this->info;
	}
	if(NULL != this->devices)
	{
		for(cl_uint i = 0 ; i < this->num_devices ; i++)
			delete this->devices[i];
		delete [] this->devices;
	}
}

cl_platform_id CCLPlatform::getID()
{
	return this->id;
}

void CCLPlatform::setID(cl_platform_id id)
{
	this->id = id;
	this->info = new CCLPlatformInfo(this->id);
}

CCLPlatformInfo * CCLPlatform::getInfo()
{
	return this->info;
}

void CCLPlatform::fillDevices(cl_device_type type)
{
	cl_uint num;
	cl_int err;
	if(NULL != this->devices)
	{
		for(cl_uint i = 0 ; i < this->num_devices ; i++)
			delete this->devices[i];
		delete [] this->devices;
		this->num_devices = 0;
		this->devices = NULL;
	}
	err = clGetDeviceIDs(this->id,type,0,NULL,&num);
	if(CL_SUCCESS != err || 0 == num)
	{
		return;
	}
	cl_device_id * device_ids = new cl_device_id[num];
	if(NULL == device_ids)
	{
		return;
	}
	err = clGetDeviceIDs(this->id,type,num,device_ids,NULL);
	if(CL_SUCCESS != err)
	{
		delete [] device_ids;
		return;
	}
	this->devices = new CCLDevice*[num];
	this->num_devices = num;
	for(cl_uint i = 0 ; i  < num ; i++)
	{
		this->devices[i] = new CCLDevice(this,device_ids[i]);
	}
}

cl_uint CCLPlatform::getDevicesCount()
{
	return this->num_devices;
}

CCLDevice CCLPlatform::getDevice(cl_uint i)
{
	if(i < this->num_devices)
	{
		return CCLDevice(*this->devices[i]);
	}
	return CCLDevice();
}

CCLDevice * CCLPlatform::getDevicePtr(cl_uint i)
{
	if(i < this->num_devices)
	{
		return this->devices[i];
	}
	return NULL;
}
