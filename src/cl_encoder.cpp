#include <cl_encoder.h>
#include <shift.h>

namespace avlib
{

CCLEncoder::CCLEncoder()
{
	m_clPolicy = new CCLFirstGPUDevicePolicy();
}

CCLEncoder::CCLEncoder(EncoderConfig cfg) : 
	CBasicEncoder(cfg)
{
	m_clPolicy = new CCLFirstGPUDevicePolicy();
}

CCLEncoder::~CCLEncoder()
{
	if(NULL != m_clPolicy)
	{
		delete m_clPolicy;
	}
}

void CCLEncoder::init(CImageFormat fmt)
{
	m_dev = this->m_clPolicy->getDevice();
	if(!m_dev.isValid())
	{
		throw utils::StringFormatException("can not get device due to policy\n");
	}
	m_dev.createContext();
	m_dev.createCommandQueue();
	cl_int err;
	std::string src = utils::get_src_from_file((char*)"src/cl/kernel.cl");
	const char * csrc = src.c_str();
	if((m_program = clCreateProgramWithSource(m_dev.getContext(), 1, &csrc, NULL, &err)) == 0 || CL_SUCCESS != err)
	{
		throw utils::StringFormatException("clCreateProgramWithSource(%d)\n", err);
	}
	if(CL_SUCCESS != (err = clBuildProgram(m_program, 0, NULL, NULL, NULL, NULL)))
	{
		size_t len;
		if(CL_SUCCESS == (err = clGetProgramBuildInfo(m_program, m_dev.getId(), CL_PROGRAM_BUILD_LOG, 0, NULL, &len)))
		{
			char * build_log = new char[len];
			if(CL_SUCCESS == (err = clGetProgramBuildInfo(m_program, m_dev.getId(), CL_PROGRAM_BUILD_LOG, len, build_log, NULL)))
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
	this->m_imgF = new CCLImage<float>(&this->m_dev, fmt);
	this->m_img = new CCLImage<int16_t>(&this->m_dev, fmt);
	this->m_dct = new CCLDCT(&this->m_dev, m_program, "dct_transform");
	this->m_quant = new CCLQuant(&this->m_dev, m_program, "quant_transform");
	this->m_zz = new CCLZigZag<float, int16_t>(&this->m_dev, m_program, "lut_transform_float_int16");
	this->m_shift = new CCLShift<float>(-128.0f, &this->m_dev, m_program, "shift");
	this->m_ishift = new CCLShift<float>(128.0f, &this->m_dev, m_program, "shift");
	this->m_pred = new CCLPrediction(&this->m_dev, fmt);
	this->m_iquant = new CCLIQuant(&this->m_dev, m_program, "iquant_transform");
	CBasicEncoder::init(fmt);
}

}
