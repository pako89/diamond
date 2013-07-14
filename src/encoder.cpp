
#include <encoder.h>

namespace avlib
{

CEncoder::CEncoder()
{
}

CEncoder::CEncoder(EncoderConfig cfg) :
	m_config(cfg)
{
}

CEncoder::~CEncoder()
{
}

sos_marker_t CEncoder::write_sos(CSequence * pSeq, CBitstream * pBstr)
{
	sos_marker_t sos;
	sos.type = MARKER_TYPE_SOS;
	sos.size = sizeof(struct sos_marker);
	switch(m_config.HuffmanType)
	{
	case HUFFMAN_TYPE_DYNAMIC:
		sos.huffman = HUFFMAN_T_DYNAMIC;
		break;
	case HUFFMAN_TYPE_STATIC:
	default:
		sos.huffman = HUFFMAN_T_STATIC;
		break;
	}
	sos.frames_number = pSeq->getFramesCount();
	sos.width = pSeq->getWidth();
	sos.height = pSeq->getHeight();
	sos.frame_rate.nom = pSeq->getFrameRate().Nom;
	sos.frame_rate.denom = pSeq->getFrameRate().Denom;
#if USE(INTERPOLATION)
	sos.interpolation_scale = m_config.InterpolationScale;
#endif
	sos.quant_coeff = m_config.Q;
	pBstr->write_block(&sos, sizeof(sos));
	return sos;
}

sof_marker_t CEncoder::write_sof(CBitstream * pBstr, FRAME_TYPE frame_type)
{
	sof_marker_t sof;
	sof.type = MARKER_TYPE_SOF;
	sof.size = sizeof(sof_marker_t);
	sof.frame_type = frame_type;
	pBstr->write_block(&sof, sizeof(sof));
	return sof;
}

void CEncoder::init(CImageFormat fmt)
{
}

bool CEncoder::Encode(CSequence * pSeq, CBitstream * pBstr)
{
	m_timer.start();
	init(pSeq->getFormat());
	sos_marker_t sos = write_sos(pSeq, pBstr);
	FRAME_TYPE frame_type;
	CImage<uint8_t> * frame = NULL;
	for(uint32_t i=0;i<sos.frames_number;i++)
	{
		sof_marker_t sof;
		frame_type = (!m_config.GOP || i%m_config.GOP == 0 || i == sos.frames_number-1)?FRAME_TYPE_I:FRAME_TYPE_P;
		sof = write_sof(pBstr, frame_type);
		if(!pSeq->ReadNext())
		{
			throw utils::StringFormatException("can not read frame from file");
		}
		frame = pSeq->getFrame();
		doEncodeFrame(frame, pBstr, frame_type);
		pBstr->flush();
		if(m_config.PrintProgressBar) printProgressBar(i, sos.frames_number);
	}
	if(m_config.PrintProgressBar) printProgressBar(sos.frames_number, sos.frames_number);
	m_timer.stop();
	if(m_config.PrintTimers)
	{
		printTimers();
	}
	return true;
}

void CEncoder::printTimers(void)
{
	log_timer("Total", m_timer);
}
	
void CEncoder::printProgressBar(int i, int n)
{
	utils::printProgressBar(i, n);
}

}
