#include <cl_device.h>
#include <cl_timers.h>

CCLDevice::CCLDevice(void) :
	m_platform(NULL),
	id(0),
	info(NULL)
{
}

CCLDevice::CCLDevice(CCLPlatform * platform, cl_device_id device_id)
{
	this->info = NULL;
	this->setId(platform, device_id);
}

CCLDevice::CCLDevice(const CCLDevice & src) :
	info(NULL)
{
	this->setId(src.m_platform, src.id);
	m_context = src.m_context;
	m_queue = src.m_queue;
}

CCLDevice::~CCLDevice(void)
{
	if(NULL != this->info)
	{
		delete this->info;
	}
}

bool CCLDevice::operator==(CCLDevice & src)
{
	return (m_queue == src.m_queue &&
		m_context == src.m_context &&
		id == src.id &&
		m_platform->getID() == src.m_platform->getID());
}

bool CCLDevice::isValid()
{
	return (m_platform != NULL && id != 0);
}

cl_platform_id CCLDevice::getPlatformId()
{
	if(NULL != m_platform)
	{
		return this->m_platform->getID();
	}
	else
	{
		throw utils::StringFormatException("m_platform = NULL\n");
	}
}

cl_device_id CCLDevice::getId()
{
	return this->id;
}

void CCLDevice::setId(CCLPlatform * platform, cl_device_id device_id)
{
	this->m_platform = platform;
	this->id = device_id;
}

CCLDeviceInfo * CCLDevice::getInfo()
{
	if(NULL == this->info)
	{
		this->info = new CCLDeviceInfo(getPlatformId(),this->id);
	}
	return this->info;
}

void CCLDevice::createContext()
{
	cl_int err;
	if((this->m_context = clCreateContext(NULL, 1, &this->id, NULL, NULL, &err)) == 0 || CL_SUCCESS != err)
	{
		throw utils::StringFormatException("clCreateContext(%d)\n", err);
	}
}

cl_context CCLDevice::getContext()
{
	if(this->m_context)
	{
		return this->m_context;
	}
	throw utils::StringFormatException("m_context = 0\n");
}

void CCLDevice::createCommandQueue()
{
	cl_int err;
	if((m_queue = clCreateCommandQueue(m_context, id, 0, &err)) == 0 || CL_SUCCESS != err)
	{
		throw utils::StringFormatException("clCreateCommandQueue(%d)\n", err);
	}
}

cl_command_queue CCLDevice::getCommandQueue()
{
	if(m_queue)
	{
		return m_queue;
	}
	throw utils::StringFormatException("m_queue = 0\n");
}

CCLPlatform * CCLDevice::getPlatform()
{
	return m_platform;
}

void CCLDevice::Finish()
{
	CCLTimers::getFinish().start();
	cl_int err = clFinish(this->m_queue);
	if(CL_SUCCESS != err) throw utils::StringFormatException("clFinish(%d)\n", err);
	CCLTimers::getFinish().stop();
}


