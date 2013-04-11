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
	CImage<float> * imgF = new CImage<float>(pSeq->getFormat());
	CImage<float> * imgLast = new CImage<float>(pSeq->getFormat());
	CImage<int16_t> * img = new CImage<int16_t>(pSeq->getFormat());
	CHuffmanTree<int16_t> * htree = new CHuffmanTree<int16_t>();
	CDCT * dct = new CDCT();
	CIDCT * idct = new CIDCT();
	CQuant * quant = new CQuant();
	CIQuant * iquant = new CIQuant();
	CZigZag<float, int16_t> * zigzag = new CZigZag<float, int16_t>();
	CRLC<int16_t> * rlc = new CRLC<int16_t>();
	sof_marker_t sof;
	sof.type = MARKER_TYPE_SOF;
	sof.size = sizeof(sof_marker_t);
	sof.quant_coeff = 1;
	int gop = 4;
	for(int i=0;i<sos.frames_number;i++)
	{
		if(!pSeq->ReadNext())
		{
			throw utils::StringFormatException("can not read frame from file");
		}
		dbg("\rEncoding frame: %d", i);
		(*imgF) = pSeq->getFrame();
		if(i%gop == 0 || i == sos.frames_number-1)
		{
			sof.frame_type = FRAME_TYPE_I;
		}
		else
		{
			sof.frame_type = FRAME_TYPE_P;
			(*imgF) -= (*imgLast);
		}	
		pBstr->write_block(&sof, sizeof(sof));
		dct->Transform(imgF, imgF);
		quant->Transform(imgF, imgF);
		zigzag->Transform(imgF, img);
		rlc->Encode(img, pBstr);
		pBstr->flush();
		iquant->Transform(imgF, imgF);
		idct->Transform(imgF, imgF);
		if(sof.frame_type == FRAME_TYPE_P)
		{
			(*imgLast) += (*imgF);
		}
		else
		{
			(*imgLast) = (*imgF);
		}
	}
	dbg("\n");
	delete imgF;
	delete imgLast;
	delete img;
	delete dct;
	delete idct;
	delete htree;
	delete quant;
	delete iquant;
	delete zigzag;
	delete rlc;
	return false;
}

}
