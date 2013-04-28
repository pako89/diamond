#ifndef _CL_ENCODER_H
#define _CL_ENCODER_H

#include <basic_encoder.h>

#include <cl_device.h>
#include <cl_image.h>
#include <cl_dct.h>
#include <cl_quantizer.h>
#include <cl_base.h>
#include <cl_policy.h>
#include <CL/opencl.h>
#include <fstream>
#include <string>

namespace avlib
{

class CCLEncoder : public CBasicEncoder
{
public:
	CCLEncoder();
	virtual ~CCLEncoder();
	virtual bool Encode(CSequence * pSeq, CBitstream * pBstr);
protected:
	virtual void init(CImageFormat fmt);
	std::string get_src_from_file(char * file_name);
	CCLDevice m_dev;
	cl_program m_program;
	CCLDevicePolicy * m_clPolicy;
};

}

#endif //_CL_ENCODER
