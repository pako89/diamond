#include <cl_parallel_encoder.h>	

namespace avlib
{

CCLParallelEncoder::CCLParallelEncoder() :
	m_clPolicy(NULL),
	m_dct(NULL),
	m_idct(NULL),
	m_quant(NULL),
	m_iquant(NULL),
	m_zz(NULL),
	m_shift(NULL),
	m_ishift(NULL),
	m_pred(NULL),
	m_predInfo(NULL),
	m_rlc(NULL),
	m_imgF(NULL),
	m_img(NULL)
{
}

CCLParallelEncoder::CCLParallelEncoder(EncoderConfig cfg) :
	CEncoder(cfg),
	m_clPolicy(NULL),
	m_dct(NULL),
	m_idct(NULL),
	m_quant(NULL),
	m_iquant(NULL),
	m_zz(NULL),
	m_shift(NULL),
	m_ishift(NULL),
	m_pred(NULL),
	m_predInfo(NULL),
	m_rlc(NULL),
	m_imgF(NULL),
	m_img(NULL)
{
}

CCLParallelEncoder::~CCLParallelEncoder()
{
	RELEASE(m_clPolicy);
	RELEASE(m_dct);
	RELEASE(m_idct);
	RELEASE(m_quant);
	RELEASE(m_iquant);
	RELEASE(m_zz);
	RELEASE(m_shift);
	RELEASE(m_ishift);
	RELEASE(m_pred);
	RELEASE(m_predInfo);
	RELEASE(m_rlc);
	RELEASE(m_imgF);
	RELEASE(m_img);
}

void CCLParallelEncoder::transform(CCLImage<float> * imgF, CCLImage<int16_t> * img, CCLPredictionInfoTable * predTab, FRAME_TYPE frame_type)
{
	m_pred->Transform(imgF, imgF, predTab, frame_type);
	m_dct->Transform(imgF, imgF);
	m_quant->Transform(imgF, imgF);
	m_zz->Transform(imgF, img);
}

void CCLParallelEncoder::itransform(CCLImage<float> * imgF, CCLImage<int16_t> * img, CCLPredictionInfoTable * predTab, FRAME_TYPE frame_type)
{
	m_iquant->Transform(imgF, imgF);
	m_idct->Transform(imgF, imgF);
	m_pred->ITransform(imgF, imgF, predTab, frame_type);
}

void CCLParallelEncoder::entropy(CCLImage<int16_t> * img, CCLPredictionInfoTable * predInfo, CBitstream * pBstr)
{
	m_pred->Encode(predInfo, pBstr);
	m_rlc->Encode(img, pBstr);
	m_rlc->Flush(pBstr);
}

bool CCLParallelEncoder::Encode(CSequence * pSeq, CBitstream * pBstr)
{
	m_timer.start();
	init(pSeq->getFormat());
	sos_marker_t sos = write_sos(pSeq, pBstr);
	m_quant->CQuant::setTables(1);
	m_iquant->CIQuant::setTables(1);
	FRAME_TYPE frame_type;
	this->m_imgF->setAutoCopy(false);
	this->m_img->setAutoCopy(false);
	this->m_pred->getLastImage()->setAutoCopy(false);
	this->m_predInfo->setAutoCopy(false);
	for(int i=0;i<sos.frames_number;i++)
	{
		if(!pSeq->ReadNext())
		{
			throw utils::StringFormatException("can not read frame from file");
		}
		utils::printProgressBar(i, sos.frames_number);
		(*(CImage<float>*)m_imgF) = pSeq->getFrame();
		m_imgF->CopyToDevice();
		sof_marker_t sof;
		frame_type = (!m_config.GOP || i%m_config.GOP == 0 || i == sos.frames_number-1)?FRAME_TYPE_I:FRAME_TYPE_P;
		sof = write_sof(pBstr, frame_type);
		transform(m_imgF, m_img, m_predInfo, frame_type);
		m_dev.Finish();
		m_img->CopyToHost();
		m_predInfo->CopyToHost();
		itransform(m_imgF, m_img, m_predInfo, frame_type);
		entropy(m_img, m_predInfo, pBstr);
		m_dev.Finish();
	}
	m_timer.stop();
	dbg("Timer total         : %f\n", m_timer.getTotalSeconds());
	dbg("Timer DCT           : %f\n", m_dct->getTimer().getTotalSeconds());
	dbg("Timer Quant         : %f\n", m_quant->getTimer().getTotalSeconds());
	dbg("Timer Zig Zag       : %f\n", m_zz->getTimer().getTotalSeconds());
	dbg("Timer RLC           : %f\n", m_rlc->getTimer().getTotalSeconds());
	return true;
}

void CCLParallelEncoder::init(CImageFormat fmt)
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
	this->m_pred = new CCLPrediction(&this->m_dev, fmt);
	this->m_pred->setTransformKernel(this->m_program, "prediction_transform");
	this->m_pred->setITransformKernel(this->m_program, "prediction_itransform");
	this->m_pred->setPredictionKernel(this->m_program, "prediction_predict");
	this->m_pred->setIFrameTransform(m_shift);
	this->m_pred->setIFrameITransform(m_ishift);
	this->m_predInfo = new CCLPredictionInfoTable(&this->m_dev, CSize(fmt.Size.Height/16, fmt.Size.Width/16));
	this->m_iquant = new CCLIQuant(&this->m_dev, this->m_program, "iquant_transform");
	if(m_config.HuffmanType == HUFFMAN_TYPE_STATIC)
	{
		m_rlc = new CStaticRLC<int16_t>();
	}
	else if(m_config.HuffmanType == HUFFMAN_TYPE_DYNAMIC)
	{
		m_rlc = new CDynamicRLC<int16_t>();
	}
	else
	{
		throw utils::StringFormatException("Unknown Huffman type\n");
	}
}

}
