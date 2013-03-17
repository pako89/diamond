#include <basic_encoder.h>
#include <log.h>

namespace avlib
{

CBasicEncoder::CBasicEncoder()
{
}

CBasicEncoder::~CBasicEncoder()
{
}

bool CBasicEncoder::Encode(CSequence * pSeq, CBitstream * pBstr)
{
	struct sos_marker sos;
	sos.type = MARKER_TYPE_SOS;
	sos.frames_number = pSeq->getFramesCount();
	sos.width = pSeq->getWidth();
	sos.height = pSeq->getHeight();
	pBstr->write_block(&sos, sizeof(sos));
	CHuffmanTree<uint8_t> htree;
	int i=0;
	while(pSeq->ReadNext())
	{
		dbg("\rEncoding frame: %d", i++);
		CImage<uint8_t> & frame = pSeq->getFrame();
		for(int i=0;i<frame.getComponents();i++)
		{
			htree.EncodeBlock(frame[i][0], frame[i].getBytesCount(), pBstr);
			pBstr->flush();
		}
	}
	dbg("\n");
	return false;
}

}
