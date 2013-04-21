#include <cl_encoder.h>

namespace avlib
{

CCLEncoder::CCLEncoder()
{
}

CCLEncoder::~CCLEncoder()
{
	if(m_h.context)
	{
		clReleaseContext(m_h.context);
	}
	if(m_h.queue)
	{
		clReleaseCommandQueue(m_h.queue);
	}
}

void CCLEncoder::init(CImageFormat fmt)
{
	cl_int dev_type = CL_DEVICE_TYPE_GPU;
	cl_int err;
	if(CL_SUCCESS != (err = clGetPlatformIDs(1, &m_h.platform_id, NULL))) 
	{
		throw utils::StringFormatException("clGetPlatformIDs(%d)\n", err);
	}
	if(CL_SUCCESS != (err = clGetDeviceIDs(m_h.platform_id, dev_type, 1, &m_h.device_id, NULL)))
	{
		throw utils::StringFormatException("clGetDeviceIDs(%d)\n", err);
	}
	if((m_h.context = clCreateContext(NULL, 1, &m_h.device_id, NULL, NULL, &err)) == 0 || CL_SUCCESS != err)
	{
		throw utils::StringFormatException("clCreateContext(%d)\n", err);
	}
	if((m_h.queue = clCreateCommandQueue(m_h.context, m_h.device_id, 0, &err)) == 0 || CL_SUCCESS != err)
	{
		throw utils::StringFormatException("clCreateCommandQueue(%d)\n", err);
	}
	std::string src = get_src_from_file((char*)"src/cl/kernel.cl");
	const char * csrc = src.c_str();
	if((m_program = clCreateProgramWithSource(m_h.context, 1, &csrc, NULL, &err)) == 0 || CL_SUCCESS != err)
	{
		throw utils::StringFormatException("clCreateProgramWithSource(%d)\n", err);
	}
	if(CL_SUCCESS != (err = clBuildProgram(m_program, 0, NULL, NULL, NULL, NULL)))
	{
		size_t len;
		if(CL_SUCCESS == (err = clGetProgramBuildInfo(m_program, m_h.device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &len)))
		{
			char * build_log = new char[len];
			if(CL_SUCCESS == (err = clGetProgramBuildInfo(m_program, m_h.device_id, CL_PROGRAM_BUILD_LOG, len, build_log, NULL)))
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
	this->m_imgF = new CCLImage<float>(this->m_h, fmt);
	this->m_dct = new CCLDCT(this->m_h, m_program, "dct_transform");
	this->m_quant = new CCLQuant(this->m_h, m_program, "quant_transform");
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
	init(pSeq->getFormat());
	sos_marker_t sos = write_sos(pSeq, pBstr);
	CIDCT * idct = new CIDCT();
	CIQuant * iquant = new CIQuant();
	int gop = 4;
	CCLImage<float> * tmpF = new CCLImage<float>(m_h, m_imgF->getFormat());
	for(int i=0;i<sos.frames_number;i++)
	{
		if(!pSeq->ReadNext())
		{
			throw utils::StringFormatException("can not read frame from file");
		}
		dbg("\rEncoding frame: %d", i);
		(*m_imgF) = pSeq->getFrame();
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
		dynamic_cast<CCLImage<float>*>(m_imgF)->CopyToDevice();
		m_dct->Transform(m_imgF, m_imgF);
		m_quant->Transform(m_imgF, m_imgF);
		dynamic_cast<CCLImage<float>*>(m_imgF)->CopyToHost();
		m_zz->Transform(m_imgF, m_img);
		m_rlc->Encode(m_img, pBstr);
		pBstr->flush();
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
	delete tmpF;
	delete idct;
	delete iquant;
	return false;
}

}
