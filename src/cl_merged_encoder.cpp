#include <cl_merged_encoder.h>	
#include <utils.h>
#include <cl_zigzag.h>
#include <cl_quantizer.h>
#include <rlc_factory.h>

namespace avlib
{

CCLMergedEncoder::CCLMergedEncoder() :
	m_clPolicy(NULL),
	m_pred(NULL),
	m_predTab(NULL),
	m_imgF(NULL),
	m_img(NULL),
	m_rlc(NULL),
	m_shift(NULL),
	m_ishift(NULL),
	m_dctqzz(NULL),
	m_idctqzz(NULL)
{
}

CCLMergedEncoder::CCLMergedEncoder(EncoderConfig cfg) :
	CEncoder(cfg),
	m_clPolicy(NULL),
	m_pred(NULL),
	m_predTab(NULL),
	m_imgF(NULL),
	m_img(NULL),
	m_rlc(NULL),
	m_shift(NULL),
	m_ishift(NULL),
	m_dctqzz(NULL),
	m_idctqzz(NULL)
{
}

CCLMergedEncoder::~CCLMergedEncoder()
{
	RELEASE(m_pred);
	RELEASE(m_predTab);
	RELEASE(m_imgF);
	RELEASE(m_img);
	RELEASE(m_rlc);
	RELEASE(m_shift);
	RELEASE(m_ishift);
	RELEASE(m_dctqzz);
	RELEASE(m_idctqzz);
}


bool CCLMergedEncoder::Encode(CSequence * pSeq, CBitstream * pBstr)
{
	m_timer.start();
	init(pSeq->getFormat());
	sos_marker_t sos = write_sos(pSeq, pBstr);
	FRAME_TYPE frame_type;
	CCLZigZag<float, int16_t> * m_zz = new CCLZigZag<float, int16_t>(&this->m_dev, this->m_program, "lut_transform_float_int16");
	m_dctqzz->setTables(1);
	m_idctqzz->setTables(1);
	for(int i=0;i<sos.frames_number;i++)
	{
		if(!pSeq->ReadNext())
		{
			throw utils::StringFormatException("can not read frame from file");
		}
		utils::printProgressBar(i, sos.frames_number);
		(*static_cast<CImage<float>*>(m_imgF)) = pSeq->getFrame();
		m_imgF->CopyToDevice();
		sof_marker_t sof;
		frame_type = (!m_config.GOP || i%m_config.GOP == 0 || i == sos.frames_number-1)?FRAME_TYPE_I:FRAME_TYPE_P;
		sof = write_sof(pBstr, frame_type);
		m_pred->Transform(m_imgF, m_imgF, m_predTab, frame_type);
		m_dctqzz->Transform(m_imgF, m_img);
		m_zz->Transform(m_imgF, m_img);
		m_pred->Encode(m_predTab, pBstr);
		m_rlc->Encode(m_img, pBstr);
		m_rlc->Flush(pBstr);
		pBstr->flush();
		m_idctqzz->Transform(m_imgF, m_imgF);
		m_pred->ITransform(m_imgF, m_imgF, m_predTab, frame_type);
	}
	delete m_zz;
	dbg("\n");
	m_timer.stop();
	dbg("Timer total         : %f\n", m_timer.getTotalSeconds());
	dbg("Timer RLC           : %f\n", m_rlc->getTimer().getTotalSeconds());
	return false;
}

void CCLMergedEncoder::init(CImageFormat fmt)
{
	this->m_clPolicy = new CCLFirstGPUDevicePolicy();
	ICLHost::init(m_clPolicy, (char*)DEFAULT_CL_SRC_FILE);
	this->m_imgF = new CCLImage<float>(&this->m_dev, fmt);
	this->m_img = new CCLImage<int16_t>(&this->m_dev, fmt);
	this->m_shift = new CCLShift<float>(-128.0f, &this->m_dev, this->m_program, "shift");
	this->m_ishift = new CCLShift<float>(128.0f, &this->m_dev, this->m_program, "shift");
	this->m_pred = new CCLPrediction(&this->m_dev, fmt);
	this->m_pred->setTransformKernel(this->m_program, "prediction_transform");
	this->m_pred->setITransformKernel(this->m_program, "prediction_itransform");
	this->m_pred->setPredictionKernel(this->m_program, "prediction_predict");
	this->m_pred->setIFrameTransform(m_shift);
	this->m_pred->setIFrameITransform(m_ishift);
	this->m_predTab = new CCLPredictionInfoTable(&this->m_dev, CSize(fmt.Size.Height/16, fmt.Size.Width/16));
	this->m_dctqzz = new CCLDCTQZZ(&this->m_dev, this->m_program, "dctqzz_transform");
	this->m_idctqzz = new CCLIDCTQ(&this->m_dev, this->m_program, "idctq_transform");
	this->m_rlc = CRLCFactory<int16_t>::CreateRLC(m_config.HuffmanType);
	if(NULL == this->m_rlc)
	{
		throw utils::StringFormatException("Unknown Huffman type '%d'\n", m_config.HuffmanType);
	}
}

}
