#include <cl_parallel_encoder.h>	

namespace avlib
{

CCLParallelEncoder::CCLParallelEncoder()
{
}

CCLParallelEncoder::CCLParallelEncoder(EncoderConfig cfg) :
	CCLEncoder(cfg)
{
}

bool CCLParallelEncoder::Encode(CSequence * pSeq, CBitstream * pBstr)
{
	m_timer.start();
	init(pSeq->getFormat());
	sos_marker_t sos = write_sos(pSeq, pBstr);
	m_quant->CQuant::setTables(1);
	m_iquant->CIQuant::setTables(1);
	FRAME_TYPE frame_type;
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
		transform(m_imgF, m_img, m_predTab, frame_type);
		m_dev.Finish();
		m_img->CopyToHost();
		m_predTab->CopyToHost();
		itransform(m_imgF, m_img, m_predTab, frame_type);
		entropy(m_img, m_predTab, pBstr, frame_type);
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
	CCLEncoder::init(fmt);
	this->m_imgF->setAutoCopy(false);
	this->m_img->setAutoCopy(false);
	this->m_predTab->setAutoCopy(false);
	this->m_pred->getLastImage()->setAutoCopy(false);
}

}
