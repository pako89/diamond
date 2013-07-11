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

void CCLMergedEncoder::doEncodeFrame(CImage<uint8_t> * pFrame, CBitstream * pBstr, FRAME_TYPE frame_type)
{
	(*static_cast<CImage<float>*>(m_imgF)) = (*pFrame);
	m_imgF->CopyToDevice();
	m_pred->Transform(m_imgF, m_imgF, m_predTab, frame_type);
	m_dctqzz->Transform(m_imgF, m_img);
	m_pred->Encode(m_predTab, pBstr, frame_type);
	m_rlc->Encode(m_img, pBstr);
	m_rlc->Flush(pBstr);
	m_idctqzz->Transform(m_imgF, m_imgF);
	m_pred->ITransform(m_imgF, m_imgF, m_predTab, frame_type);
}

void CCLMergedEncoder::init(CImageFormat fmt)
{
	this->m_clPolicy = new CCLFirstGPUDevicePolicy();
	ICLHost::init(m_clPolicy, (char*)DEFAULT_CL_SRC_FILE);
	this->m_imgF = new CCLImage<float>(&this->m_dev, fmt);
	this->m_img = new CCLImage<int16_t>(&this->m_dev, fmt);
	this->m_shift = new CCLShift<float>(-128.0f, &this->m_dev, this->m_program, "shift");
	this->m_ishift = new CCLShift<float>(128.0f, &this->m_dev, this->m_program, "shift");
	this->m_pred = new CCLPrediction(&this->m_dev);
#if USE(INTERPOLATION)
	this->m_pred->Init(fmt, m_config.InterpolationScale, this->m_program, "interpolation_float");
	this->m_pred->setTransformKernel(this->m_program, "prediction_transform_interpolation");
	this->m_pred->setITransformKernel(this->m_program, "prediction_itransform_interpolation");
	this->m_pred->setPredictionKernel(this->m_program, "prediction_predict_interpolation");
#else
	this->m_pred->Init(fmt);
	this->m_pred->setTransformKernel(this->m_program, "prediction_transform");
	this->m_pred->setITransformKernel(this->m_program, "prediction_itransform");
	this->m_pred->setPredictionKernel(this->m_program, "prediction_predict");
#endif
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
	m_dctqzz->setTables(1);
	m_idctqzz->setTables(1);
}

}
