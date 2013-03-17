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
	CHuffmanTree<uint8_t> htree;
	pBstr->fill();
	for(uint32_t i = 0 ; i < sos.frames_number; i++)
	{
		dbg("\rDecoding frame: %d", i);
		CImage<uint8_t> & frame = pSeq->getFrame();
		for(int i=0;i<frame.getComponents();i++)
		{
			htree.DecodeBlock(frame[i][0], frame[i].getBytesCount(), pBstr);
		}
		if(!pSeq->WriteNext())
		{
			throw utils::StringFormatException("can not write frame to file");
		}
	}
	dbg("\n");
	return false;
}

}
