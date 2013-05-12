#include <basic_encoder.h>
#include <log.h>

module("BasicEncoder");

namespace avlib
{

CBasicEncoder::CBasicEncoder() :
	m_imgF(NULL),
	m_imgLast(NULL),
	m_img(NULL),
	m_htree(NULL),
	m_dct(NULL),
	m_quant(NULL),
	m_zz(NULL),
	m_rlc(NULL)
{
}

#define RELEASE(p)	if(NULL != (p)) delete (p);

CBasicEncoder::~CBasicEncoder()
{
	RELEASE(m_imgF);
	RELEASE(m_imgLast);
	RELEASE(m_img);
	RELEASE(m_htree);
	RELEASE(m_dct);
	RELEASE(m_quant);
	RELEASE(m_rlc);
}

void CBasicEncoder::init(CImageFormat fmt)
{
	if(NULL == m_imgF) m_imgF = new CImage<float>(fmt);
	if(NULL == m_imgLast) m_imgLast = new CImage<float>(fmt);
	if(NULL == m_img) m_img = new CImage<int16_t>(fmt);
	if(NULL == m_htree) m_htree = new CDynamicHuffman<int16_t>();
	if(NULL == m_dct) m_dct = new CDCT();
	if(NULL == m_quant) m_quant = new CQuant();
	if(NULL == m_zz) m_zz = new CZigZag<float, int16_t>();
	if(NULL == m_rlc) m_rlc = new CRLC<int16_t>();
}

sos_marker_t CBasicEncoder::write_sos(CSequence * pSeq, CBitstream * pBstr)
{
	sos_marker_t sos;
	sos.type = MARKER_TYPE_SOS;
	sos.size = sizeof(struct sos_marker);
	sos.frames_number = pSeq->getFramesCount();
	sos.width = pSeq->getWidth();
	sos.height = pSeq->getHeight();
	pBstr->write_block(&sos, sizeof(sos));
	return sos;
}

sof_marker_t CBasicEncoder::write_sof(CBitstream * pBstr, frame_type_t frame_type)
{
	sof_marker_t sof;
	sof.type = MARKER_TYPE_SOF;
	sof.size = sizeof(sof_marker_t);
	sof.quant_coeff = 1;
	sof.frame_type = frame_type;
	pBstr->write_block(&sof, sizeof(sof));
	return sof;
}

bool CBasicEncoder::Encode(CSequence * pSeq, CBitstream * pBstr)
{
	m_timer.start();
	init(pSeq->getFormat());
	sos_marker_t sos = write_sos(pSeq, pBstr);
	CIDCT * idct = new CIDCT();
	CIQuant * iquant = new CIQuant();
	int gop = 4;
	m_quant->setTables(1);
	for(int i=0;i<sos.frames_number;i++)
	{
		if(!pSeq->ReadNext())
		{
			throw utils::StringFormatException("can not read frame from file");
		}
		dbg("\rEncoding frame: %d/%d", i, sos.frames_number);
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
		m_dct->Transform(m_imgF, m_imgF);
		m_quant->Transform(m_imgF, m_imgF);
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
	m_timer.stop();
	dbg("Timer total         : %f\n", m_timer.getTotalSeconds());
	dbg("Timer DCT           : %f\n", m_dct->getTimer().getTotalSeconds());
	dbg("Timer Quant         : %f\n", m_quant->getTimer().getTotalSeconds());
	dbg("Timer Zig Zag       : %f\n", m_zz->getTimer().getTotalSeconds());
	dbg("Timer RLC           : %f\n", m_rlc->getTimer().getTotalSeconds());
	delete idct;
	delete iquant;
	return false;
}

}
