#ifndef _CL_ENCODER_H
#define _CL_ENCODER_H

#include <basic_encoder.h>
#include <cl_common.h>
#include <cl_image.h>
#include <cl_dct.h>
#include <cl_quantizer.h>
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
	cl_handle m_h;
	cl_program m_program;
};

}

#endif //_CL_ENCODER
