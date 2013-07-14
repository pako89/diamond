#include <basic_encoder.h>
#include <log.h>
#include <shift.h>


namespace avlib
{

CBasicEncoder::CBasicEncoder() :
	m_imgF(NULL),
	m_imgLast(NULL),
	m_img(NULL),
	m_dct(NULL),
	m_quant(NULL),
	m_zz(NULL),
	m_rlc(NULL),
	m_shift(NULL),
	m_ishift(NULL),
	m_idct(NULL),
	m_iquant(NULL),
	m_pred(NULL),
	m_predTab(NULL)
{
}

CBasicEncoder::CBasicEncoder(EncoderConfig cfg) :
	CEncoder(cfg),
	m_imgF(NULL),
	m_imgLast(NULL),
	m_img(NULL),
	m_dct(NULL),
	m_quant(NULL),
	m_zz(NULL),
	m_rlc(NULL),
	m_shift(NULL),
	m_ishift(NULL),
	m_idct(NULL),
	m_iquant(NULL),
	m_pred(NULL),
	m_predTab(NULL)
{
}

CBasicEncoder::~CBasicEncoder()
{
	RELEASE(m_imgF);
	RELEASE(m_imgLast);
	RELEASE(m_img);
	RELEASE(m_dct);
	RELEASE(m_quant);
	RELEASE(m_rlc);
	RELEASE(m_shift);
	RELEASE(m_ishift);
	RELEASE(m_idct);
	RELEASE(m_iquant);
	RELEASE(m_pred);
	RELEASE(m_predTab);
}

void CBasicEncoder::init(CImageFormat fmt)
{
	if(NULL == m_imgF) m_imgF = new CImage<float>(fmt);
	if(NULL == m_imgLast) m_imgLast = new CImage<float>(fmt);
	if(NULL == m_img) m_img = new CImage<int16_t>(fmt);
	if(NULL == m_dct) m_dct = new CDCT();
	if(NULL == m_quant) m_quant = new CQuant();
	if(NULL == m_zz) m_zz = new CZigZag<float, int16_t>();
	if(NULL == m_rlc)
	{
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
	if(NULL == m_shift) m_shift = new CShift<float>(-128.0f);
	if(NULL == m_ishift) m_ishift = new CShift<float>(128.0f);
	if(NULL == m_idct) m_idct = new CIDCT();
	if(NULL == m_iquant) m_iquant = new CIQuant();
	if(NULL == m_pred)
	{
		m_pred = new CPrediction();
#if USE(INTERPOLATION)
		m_pred->Init(fmt, m_config.InterpolationScale);
#else
		m_pred->Init(fmt);
#endif
	}
	if(NULL == m_predTab) m_predTab = new CPredictionInfoTable(CSize(fmt.Size.Height/16, fmt.Size.Width/16));
	m_pred->setIFrameTransform(m_shift);
	m_pred->setIFrameITransform(m_ishift);
	m_quant->setTables(m_config.Q);
	m_iquant->setTables(m_config.Q);
}

void CBasicEncoder::doEncodeFrame(CImage<uint8_t> * pFrame, CBitstream * pBstr, FRAME_TYPE frame_type)
{
	(*m_imgF) = (*pFrame);
	m_pred->Transform(m_imgF, m_imgF, m_predTab, frame_type);
	m_dct->Transform(m_imgF, m_imgF);
	m_quant->Transform(m_imgF, m_imgF);
	m_zz->Transform(m_imgF, m_img);
	m_pred->Encode(m_predTab, pBstr, frame_type);
	m_rlc->Encode(m_img, pBstr);
	m_rlc->Flush(pBstr);
	pBstr->flush();
	m_iquant->Transform(m_imgF, m_imgF);
	m_idct->Transform(m_imgF, m_imgF);
	m_pred->ITransform(m_imgF, m_imgF, m_predTab, frame_type);
}


void CBasicEncoder::printTimers(void)
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
}


}
