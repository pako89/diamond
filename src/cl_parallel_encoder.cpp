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

void CCLParallelEncoder::doEncodeFrame(CImage<uint8_t> * pFrame, CBitstream * pBstr, FRAME_TYPE frame_type)
{
	(*(CImage<float>*)m_imgF) = (*pFrame);
	m_imgF->CopyToDevice();
	transform(m_imgF, m_img, m_predTab, frame_type);
	m_dev.Finish();
	m_img->CopyToHost();
	m_predTab->CopyToHost();
	itransform(m_imgF, m_img, m_predTab, frame_type);
	entropy(m_img, m_predTab, pBstr, frame_type);
	m_dev.Finish();
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
