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
	this->m_dev = this->m_clPolicy->getDevice();
	if(!this->m_dev.isValid())
	{
		throw utils::StringFormatException("can not get device due to policy\n");
	}
	this->m_dev.createContext();
	this->m_dev.createCommandQueue();
	cl_int err;
	std::string src = utils::get_src_from_file((char*)"src/cl/kernel.cl");
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
	this->m_imgF = new CCLImage<float>(&this->m_dev, fmt);
	this->m_img = new CCLImage<int16_t>(&this->m_dev, fmt);
	this->m_dct = new CCLDCT(&this->m_dev, this->m_program, "dct_transform");
	this->m_idct = new CCLIDCT(&this->m_dev, this->m_program, "idct_transform");
	this->m_quant = new CCLQuant(&this->m_dev, this->m_program, "quant_transform");
	this->m_zz = new CCLZigZag<float, int16_t>(&this->m_dev, this->m_program, "lut_transform_float_int16");
	this->m_shift = new CCLShift<float>(-128.0f, &this->m_dev, this->m_program, "shift");
	this->m_ishift = new CCLShift<float>(128.0f, &this->m_dev, this->m_program, "shift");
	this->m_pred = new CCLPrediction(&this->m_dev, fmt);
	this->m_iquant = new CCLIQuant(&this->m_dev, this->m_program, "iquant_transform");
	CBasicEncoder::init(fmt);
}

bool CCLEncoder::Encode(CSequence * pSeq, CBitstream * pBstr)
{
#ifndef DEBUG
	return CBasicEncoder::Encode(pSeq, pBstr);
#else
	m_timer.start();
	init(pSeq->getFormat());
	sos_marker_t sos = write_sos(pSeq, pBstr);
	m_pred->setIFrameTransform(m_shift);
	m_pred->setIFrameITransform(m_ishift);
	m_quant->setTables(1);
	m_iquant->setTables(1);
	FRAME_TYPE frame_type;
	for(int i=0;i<sos.frames_number;i++)
	{
		if(!pSeq->ReadNext())
		{
			throw utils::StringFormatException("can not read frame from file");
		}
		utils::printProgressBar(i, sos.frames_number);
		(*m_imgF) = pSeq->getFrame();
		sof_marker_t sof;
		frame_type = (!m_config.GOP || i%m_config.GOP == 0 || i == sos.frames_number-1)?FRAME_TYPE_I:FRAME_TYPE_P;
		sof = write_sof(pBstr, frame_type);
		m_pred->Transform(m_imgF, m_imgF, m_predTab, frame_type);
		m_dct->Transform(m_imgF, m_imgF);
		m_quant->Transform(m_imgF, m_imgF);
		m_zz->Transform(m_imgF, m_img);
		m_rlc->Encode(m_img, m_predTab, pBstr);
		m_rlc->Flush(pBstr);
		pBstr->flush();
		m_iquant->Transform(m_imgF, m_imgF);
		m_idct->Transform(m_imgF, m_imgF);
		m_pred->ITransform(m_imgF, m_imgF, m_predTab, frame_type);
	}
	dbg("\n");
	m_timer.stop();
	dbg("Timer total         : %f\n", m_timer.getTotalSeconds());
	dbg("Timer DCT           : %f\n", m_dct->getTimer().getTotalSeconds());
	dbg("Timer Quant         : %f\n", m_quant->getTimer().getTotalSeconds());
	dbg("Timer Zig Zag       : %f\n", m_zz->getTimer().getTotalSeconds());
	dbg("Timer RLC           : %f\n", m_rlc->getTimer().getTotalSeconds());
	return false;
#endif
}

}
