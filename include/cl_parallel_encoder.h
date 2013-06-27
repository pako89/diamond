#ifndef _CL_PARALLEL_ENCODER_H
#define _CL_PARALLEL_ENCODER_H

#ifndef DEFAULT_CL_SRC_FILE
#define DEFAULT_CL_SRC_FILE "kernel.cl"
#endif

#include <avlib.h>
#include <utils.h>
#include <encoder.h>
#include <cl_host.h>
#include <cl_dct.h>
#include <cl_quantizer.h>
#include <cl_zigzag.h>
#include <rlc.h>
#include <static_rlc.h>
#include <dynamic_rlc.h>
#include <cl_shift.h>
#include <cl_prediction.h>
#include <cl_image.h>
#include <cl_encoder.h>

namespace avlib
{

class CCLParallelEncoder : public CCLEncoder
{
public:
	CCLParallelEncoder();
	CCLParallelEncoder(EncoderConfig cfg);
	virtual bool Encode(CSequence * pSeq, CBitstream * pBstr);
protected:	
	virtual void init(CImageFormat fmt);
};

}

#endif // _CL_PARALLEL_ENCODER_H
