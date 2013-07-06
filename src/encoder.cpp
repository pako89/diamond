
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
#if USE(INTERPOLATION)
	sos.interpolation_scale = m_config.InterpolationScale;
#endif
	pBstr->write_block(&sos, sizeof(sos));
	return sos;
}

sof_marker_t CEncoder::write_sof(CBitstream * pBstr, FRAME_TYPE frame_type)
{
	sof_marker_t sof;
	sof.type = MARKER_TYPE_SOF;
	sof.size = sizeof(sof_marker_t);
	sof.quant_coeff = 1;
	sof.frame_type = frame_type;
	pBstr->write_block(&sof, sizeof(sof));
	return sof;
}

void CEncoder::init(CImageFormat fmt)
{
}

}
