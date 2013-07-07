#include <basic_decoder.h>

#include <utils.h>
#include <log.h>
#include <shift.h>

namespace avlib
{

CBasicDecoder::CBasicDecoder(bool progressBar) :
	CDecoder(progressBar)
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
	dbg("Frames : %d\n", sos.frames_number);
	dbg("Width  : %d\n", sos.width);
	dbg("Height : %d\n", sos.height);
	dbg("Huffman: %s\n", sos.huffman==HUFFMAN_T_DYNAMIC?"dynamic":"static");
	pSeq->setFormat(IMAGE_TYPE_YUV420, sos.height, sos.width);
	pBstr->fill();
	CImage<float> * imgF = new CImage<float>(pSeq->getFormat());
	CImage<float> * imgLast = new CImage<float>(pSeq->getFormat());
	CImage<int16_t> * img = new CImage<int16_t>(pSeq->getFormat());
	CDynamicHuffman<int16_t> * htree = new CDynamicHuffman<int16_t>();
	CIDCT * idct = new CIDCT();
	CIQuant * iquant = new CIQuant();
	iquant->setTables(1);
	CIZigZag<int16_t, float> * izigzag = new CIZigZag<int16_t, float>();
	CIRLC<int16_t> * irlc;
	CPredictionInfoTable * predTab = new CPredictionInfoTable(CSize(pSeq->getFormat().Size.Height/16, pSeq->getFormat().Size.Width/16));
	switch(sos.huffman)
	{
	case HUFFMAN_T_DYNAMIC:
		irlc = new CDynamicIRLC<int16_t>();
		break;
	case HUFFMAN_T_STATIC:
	default:
		irlc = new CStaticIRLC<int16_t>();
		break;
	}
	CShift<float> * shift = new CShift<float>(128.0f);
	CPrediction * pred = new CPrediction();
#if USE(INTERPOLATION)
	pred->Init(pSeq->getFormat(), sos.interpolation_scale);
#else
	pred->Init(pSeq->getFormat());
#endif
	pred->setIFrameITransform(shift);
	sof_marker_t sof;
	for(uint32_t n = 0 ; n < sos.frames_number; n++)
	{
		printProgressBar(n, sos.frames_number);
		pBstr->fill();
		pBstr->read_block(&sof, sizeof(sof));
		if(sof.type != MARKER_TYPE_SOF || sof.size != sizeof(sof_marker_t))
		{
			throw utils::StringFormatException("can not sync frame");
		}
		pred->Decode(predTab, pBstr, (FRAME_TYPE)sof.frame_type);
		irlc->Decode(pBstr, img);
		irlc->Fill(pBstr);
		izigzag->Transform(img, imgF);
		iquant->Transform(imgF, imgF);
		idct->Transform(imgF, imgF);
		pred->ITransform(imgF, imgF, predTab, (FRAME_TYPE)sof.frame_type);
		pSeq->getFrame() = *imgF;
		if(!pSeq->WriteNext())
		{
			throw utils::StringFormatException("can not write frame to file");
		}
		(*imgLast) = (*imgF);
	}
	dbg("\n");
	delete shift;
	delete pred;
	delete predTab;
	delete img;
	delete imgF;
	delete imgLast;
	delete idct;
	delete htree;
	delete izigzag;
	delete iquant;
	delete irlc;
	return false;
}

}
