#include <cl_encoder.h>
#include <shift.h>
#include <cl_timers.h>

namespace avlib
{

CCLEncoder::CCLEncoder() : 
	m_clPolicy(NULL)
{
}

CCLEncoder::CCLEncoder(EncoderConfig cfg) : 
	CEncoder(cfg),
	m_clPolicy(NULL)
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
	m_clPolicy = new CCLFirstGPUDevicePolicy();
	ICLHost::init(m_clPolicy, (char*)DEFAULT_CL_SRC_FILE);
	this->m_imgF = new CCLImage<float>(&this->m_dev, fmt);
	this->m_img = new CCLImage<int16_t>(&this->m_dev, fmt);
	this->m_dct = new CCLDCT(&this->m_dev, this->m_program, "dct_transform");
	this->m_idct = new CCLIDCT(&this->m_dev, this->m_program, "idct_transform");
	this->m_quant = new CCLQuant(&this->m_dev, this->m_program, "quant_transform");
	this->m_zz = new CCLZigZag<float, int16_t>(&this->m_dev, this->m_program, "lut_transform_float_int16");
	this->m_shift = new CCLShift<float>(-128.0f, &this->m_dev, this->m_program, "shift");
	this->m_ishift = new CCLShift<float>(128.0f, &this->m_dev, this->m_program, "shift");
	this->m_pred = new CCLPrediction(&this->m_dev);
	this->m_pred->Init(fmt, m_config.InterpolationScale, this->m_program, "interpolation_float");
	this->m_pred->setTransformKernel(this->m_program, "prediction_transform_interpolation");
	this->m_pred->setITransformKernel(this->m_program, "prediction_itransform_interpolation");
	this->m_pred->setPredictionKernel(this->m_program, "prediction_predict_interpolation");
	this->m_pred->setIFrameTransform(m_shift);
	this->m_pred->setIFrameITransform(m_ishift);
	this->m_predTab = new CCLPredictionInfoTable(&this->m_dev, CSize(fmt.Size.Height/16, fmt.Size.Width/16));
	this->m_iquant = new CCLIQuant(&this->m_dev, this->m_program, "iquant_transform");
	this->m_rlc = CRLCFactory<int16_t>::CreateRLC(m_config.HuffmanType);
	if(NULL == this->m_rlc)
	{
		throw utils::StringFormatException("Unknown Huffman type '%d'\n", m_config.HuffmanType);
	}
	m_pred->setIFrameTransform(m_shift);
	m_pred->setIFrameITransform(m_ishift);
	m_quant->setTables(m_config.Q);
	m_iquant->setTables(m_config.Q);
}

void CCLEncoder::transform(CCLImage<float> * imgF, CCLImage<int16_t> * img, CCLPredictionInfoTable * predTab, FRAME_TYPE frame_type)
{
	m_pred->Transform(imgF, imgF, predTab, frame_type);
	m_dct->Transform(imgF, imgF);
	m_quant->Transform(imgF, imgF);
	m_zz->Transform(imgF, img);
}

void CCLEncoder::itransform(CCLImage<float> * imgF, CCLImage<int16_t> * img, CCLPredictionInfoTable * predTab, FRAME_TYPE frame_type)
{
	m_iquant->Transform(imgF, imgF);
	m_idct->Transform(imgF, imgF);
	m_pred->ITransform(imgF, imgF, predTab, frame_type);
}

void CCLEncoder::entropy(CCLImage<int16_t> * img, CCLPredictionInfoTable * predInfo, CBitstream * pBstr, FRAME_TYPE frame_type)
{
	m_pred->Encode(predInfo, pBstr, frame_type);
	m_rlc->Encode(img, pBstr);
	m_rlc->Flush(pBstr);
	pBstr->flush();
}

void CCLEncoder::doEncodeFrame(CImage<uint8_t> * pFrame, CBitstream * pBstr, FRAME_TYPE frame_type)
{
	(*static_cast<CImage<float>*>(m_imgF)) = (*pFrame);
	m_imgF->CopyToDevice();
	transform(m_imgF, m_img, m_predTab, frame_type);
	entropy(m_img, m_predTab, pBstr, frame_type);
	itransform(m_imgF, m_img, m_predTab, frame_type);
}

void CCLEncoder::printTimers(void)
{
	CEncoder::printTimers();
	log_timer("DCT", m_dct->getTimer());
	log_timer("IDCT", m_idct->getTimer());
	log_timer("Quant", m_quant->getTimer());
	log_timer("Inverse Quant", m_iquant->getTimer());
	log_timer("Zig Zag", m_zz->getTimer());
	log_timer("RLC", m_rlc->getTimer());
	log_timer("Prediction", m_pred->getTimer(CPrediction::PredictionTimer_Prediction));
	log_timer("P FRAME Transform", m_pred->getTimer(CPrediction::PredictionTimer_Transform));
	log_timer("P FRAME ITransform", m_pred->getTimer(CPrediction::PredictionTimer_ITransform));
	log_timer("Interpolation", m_pred->getTimer(CPrediction::PredictionTimer_Interpolation));
	log_timer("Copy Last Image", m_pred->getTimer(CPrediction::PredictionTimer_CopyLast));
	log_timer("Encode Prediction", m_pred->getTimer(CPrediction::PredictionTimer_EncodePrediction));
	log_timer("Shift +128", m_shift->getTimer());
	log_timer("Shift -128", m_ishift->getTimer());
	CCLTimers::Print();
}

}
