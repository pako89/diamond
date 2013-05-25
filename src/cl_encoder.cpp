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
	std::string src = this->get_src_from_file((char*)"src/cl/kernel.cl");
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
	this->m_quant->setTables(1);
	CBasicEncoder::init(fmt);
}

std::string CCLEncoder::get_src_from_file(char * file_name)
{
	std::fstream fs(file_name,(std::fstream::in | std::fstream::binary));
	std::string s;
	if(fs.is_open()) 
	{
		size_t file_size;
		fs.seekg(0,std::fstream::end);
		file_size = (size_t)fs.tellg();
		fs.seekg(0,std::fstream::beg);
		char * str = new char[file_size+1];
		str[file_size] = '\0';
		if(NULL == str) 
		{
			fs.close();
			return std::string();
		}
		fs.read(str,file_size);
		fs.close();
		s = str;
		delete [] str;
	} 
	else 
	{
		throw utils::StringFormatException("Can not open file: %s\n", file_name);
	}			            
	return s;
}

bool CCLEncoder::Encode(CSequence * pSeq, CBitstream * pBstr)
{
#if 0
	init(pSeq->getFormat());
	sos_marker_t sos = write_sos(pSeq, pBstr);
#else
	m_timer.start();
	init(pSeq->getFormat());
	sos_marker_t sos = write_sos(pSeq, pBstr);
	CIDCT * idct = new CCLIDCT(&this->m_dev, m_program, "idct_transform");
	CIQuant * iquant = new CIQuant();
	int gop = 4;
	CCLImage<float> * tmpF = new CCLImage<float>(&m_dev, m_imgF->getFormat());
	CShift<float> * shift = new CShift<float>(-128.0f);
	for(int i=0;i<sos.frames_number;i++)
	{
		if(!pSeq->ReadNext())
		{
			throw utils::StringFormatException("can not read frame from file");
		}
		dbg("\rEncoding frame: %d", i);
		(*m_imgF) = pSeq->getFrame();
		//shift->Transform(m_imgF, m_imgF);
		sof_marker_t sof;
		if(i%gop == 0 || i == sos.frames_number-1)
		{
			sof = write_sof(pBstr, FRAME_TYPE_I);
		}
		else
		{
			sof = write_sof(pBstr, FRAME_TYPE_P);
			(*m_imgF) -= (*m_imgLast);
		}
		m_timerCopyToDevice.start();
		dynamic_cast<CCLImage<float>*>(m_imgF)->CopyToDevice();
		m_timerCopyToDevice.stop();
		m_dct->Transform(m_imgF, m_imgF);
		m_quant->Transform(m_imgF, m_imgF);
		m_timerCopyToHost.start();
		m_zz->Transform(m_imgF, m_img);
		dynamic_cast<CCLImage<int16_t>*>(m_img)->CopyToHost();
		m_timerCopyToHost.stop();
		m_rlc->Encode(m_img, pBstr);
		pBstr->flush();
		dynamic_cast<CCLImage<float>*>(m_imgF)->CopyToHost();
		iquant->Transform(m_imgF, m_imgF);
		idct->Transform(m_imgF, m_imgF);
		if(sof.frame_type == FRAME_TYPE_P)
		{
			(*m_imgLast) += (*m_imgF);
		}
		else
		{
			(*m_imgLast) = (*m_imgF);
		}
	}
	dbg("\n");
	m_timer.stop();
	dbg("Timer total         : %f\n", m_timer.getTotalSeconds());
	dbg("Timer Copy To Device: %f\n", m_timerCopyToDevice.getTotalSeconds());
	dbg("Timer Copy To Host  : %f\n", m_timerCopyToHost.getTotalSeconds());
	dbg("Timer DCT           : %f\n", m_dct->getTimer().getTotalSeconds());
	dbg("Timer Quant         : %f\n", m_quant->getTimer().getTotalSeconds());
	dbg("Timer Zig Zag       : %f\n", m_zz->getTimer().getTotalSeconds());
	dbg("Timer RLC           : %f\n", m_rlc->getTimer().getTotalSeconds());
	delete tmpF;
	delete idct;
	delete iquant;
	delete shift;
	return false;
#endif
}

}
