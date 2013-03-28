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
	sos.size = sizeof(struct sos_marker);
	sos.frames_number = pSeq->getFramesCount();
	sos.width = pSeq->getWidth();
	sos.height = pSeq->getHeight();
	pBstr->write_block(&sos, sizeof(sos));
	CImage<float> * imgIn = new CImage<float>(pSeq->getFormat());
	CImage<float> * imgDCT = new CImage<float>(pSeq->getFormat());
	CImage<float> * imgLast = new CImage<float>(pSeq->getFormat());
	CImage<int32_t> * img = new CImage<int32_t>(pSeq->getFormat());
	CHuffmanTree<int32_t> * htree = new CHuffmanTree<int32_t>();
	CDCT * dct = new CDCT();
	CQuant * quant = new CQuant();
	CZigZag<float, int32_t> * zigzag = new CZigZag<float, int32_t>();
	int n=0;
	struct sof_marker sof;
	sof.type = MARKER_TYPE_SOF;
	sof.size = sizeof(struct sof_marker);
	sof.quant_coeff = 1;
	while(pSeq->ReadNext())
	{
		dbg("\rEncoding frame: %d", n);
		CImage<uint8_t> & frame = pSeq->getFrame(); 
		(*imgIn) = frame;
		(*imgDCT) = (*imgIn);
		if(n == 0)
		{
			sof.frame_type = FRAME_TYPE_I;
		}
		else
		{
			sof.frame_type = FRAME_TYPE_P;
			(*imgDCT) -= (*imgLast);
		}
		dct->Transform(imgDCT, imgDCT);
		quant->Transform(imgDCT, imgDCT);
		zigzag->Transform(imgDCT, img);
		for(int i=0;i<img->getComponents();i++)
		{
			htree->EncodeBlock(&(*img)[i][0][0], (*img)[i].getPointsCount(), pBstr);
			pBstr->flush();
		}
		(*imgLast) = (*imgIn);
		n++;
	}
	dbg("\n");
	delete imgIn;
	delete imgDCT;
	delete imgLast;
	delete dct;
	delete img;
	delete htree;
	delete quant;
	delete zigzag;
	return false;
}

}
