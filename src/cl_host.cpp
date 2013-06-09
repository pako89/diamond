#include <cl_host.h>
	
ICLHost::ICLHost()
{
}

ICLHost::~ICLHost()
{
}

void ICLHost::init(CCLDevicePolicy * policy, char * fileName)
{
	this->m_dev = policy->getDevice();
	if(!this->m_dev.isValid())
	{
		throw utils::StringFormatException("can not get device due to policy\n");
	}
	this->m_dev.createContext();
	this->m_dev.createCommandQueue();
	cl_int err;
	std::string src = utils::get_src_from_file(fileName);
	const char * csrc = src.c_str();
	if((this->m_program = clCreateProgramWithSource(this->m_dev.getContext(), 1, &csrc, NULL, &err)) == 0 || CL_SUCCESS != err)
	{
		throw utils::StringFormatException("clCreateProgramWithSource(%d)\n", err);
	}
	if(CL_SUCCESS != (err = clBuildProgram(this->m_program, 0, NULL, NULL, NULL, NULL)))
	{
		size_t len;
		if(CL_SUCCESS == (err = clGetProgramBuildInfo(this->m_program, this->m_dev.getId(), CL_PROGRAM_BUILD_LOG, 0, NULL, &len)))
		{
			char * build_log = new char[len];
			if(CL_SUCCESS == (err = clGetProgramBuildInfo(this->m_program, this->m_dev.getId(), CL_PROGRAM_BUILD_LOG, len, build_log, NULL)))
			{
				throw utils::StringFormatException("Build log: %s\n", build_log);
			}
			else
			{
				throw utils::StringFormatException("clGetProgramBuildInfo(%d)\n", err);
			}
			delete [] build_log;
		}
		else
		{
			throw utils::StringFormatException("clGetProgramBuildInfo(%d)\n", err);
		}
	}
}

