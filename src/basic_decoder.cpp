#include <basic_decoder.h>

#include <utils.h>
#include <log.h>

namespace avlib
{

CBasicDecoder::CBasicDecoder()
{
}

CBasicDecoder::~CBasicDecoder()
{
}

bool CBasicDecoder::Decode(CBitstream * pBstr, CSequence * pSeq)
{	
	struct sos_marker sos;
	pBstr->read_block(&sos, sizeof(sos));
	if(MARKER_TYPE_SOS != sos.type)
	{
		throw utils::StringFormatException("wrong stream format [can not find start of stream marker]\n");
	}
	dbg("Start of Sequence marker:\n");
	dbg("Frames: %d\n", sos.frames_number);
	dbg("Width : %d\n", sos.width);
	dbg("Height: %d\n", sos.height);
	pSeq->setFormat(IMAGE_TYPE_YUV420, sos.height, sos.width);
	pBstr->fill();
	CImage<float> * imgOut = new CImage<float>(pSeq->getFormat());
	CImage<float> * imgDCT = new CImage<float>(pSeq->getFormat());
	CImage<float> * imgLast = new CImage<float>(pSeq->getFormat());
	CImage<int32_t> * img = new CImage<int32_t>(pSeq->getFormat());
	CHuffmanTree<int32_t> * htree = new CHuffmanTree<int32_t>();
	CIDCT * idct = new CIDCT();
	for(uint32_t n = 0 ; n < sos.frames_number; n++)
	{
		dbg("\rDecoding frame: %d", n);
		for(int i=0;i<img->getComponents(); i++)
		{
			//pBstr->read_block(&(*img)[i][0][0], (*img)[i].getBytesCount());
			htree->DecodeBlock(&(*img)[i][0][0], (*img)[i].getPointsCount(), pBstr);
			pBstr->fill();
		}
		(*imgDCT) = (*img);
		idct->Transform(imgDCT, imgOut);
		if(n != 0)
		{
			(*imgOut) += (*imgLast);
		}
		CImage<uint8_t> & frame = pSeq->getFrame();
		frame = *imgOut;
		if(!pSeq->WriteNext())
		{
			throw utils::StringFormatException("can not write frame to file");
		}
		(*imgLast) = (*imgOut);
	}
	dbg("\n");
	delete idct;
	delete imgOut;
	delete imgDCT;
	delete img;
	delete imgLast;
	delete htree;
	return false;
}

}
