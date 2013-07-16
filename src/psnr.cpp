#include <psnr.h>
#include <math.h>

namespace utils
{

PSNRResultItem<float> PSNRResult::getMean()
{
	PSNRResultItem<float> ret;
	ret.Y = Sum.Y/Count.Y;
	ret.U = Sum.U/Count.U;
	ret.V = Sum.V/Count.V;
	return ret;
}

PSNRResult & PSNRResult::operator+=(const PSNRResultItem<float> & r)
{
	Sum.Y += r.Y;
	Sum.U += r.U;
	Sum.V += r.V;
	Count.Y++;
	Count.U++;
	Count.V++;
	return *this;
}

CPSNR::CPSNR(PSNRConfig cfg) :
	m_config(cfg)
{
}

CPSNR::~CPSNR()
{
}

PSNRResults CPSNR::Compute(avlib::CSequence & seq1, avlib::CSequence & seq2)
{
	PSNRResults results;
	int n = utils::min(seq1.getFramesCount(), seq2.getFramesCount());
	for(int i=0;i<n;i++)
	{
		if(!seq1.ReadNext())
		{
			throw utils::StringFormatException("can not read frame from file: %s", m_config.Seq[0].FileName);
		}
		if(!seq2.ReadNext())
		{
			throw utils::StringFormatException("can not read frame from file: %s", m_config.Seq[1].FileName);
		}
		avlib::FRAME_TYPE frame_type = 
			(!m_config.GOP || i%m_config.GOP == 0)?
			avlib::FRAME_TYPE_I:
			avlib::FRAME_TYPE_P;
		avlib::CImage<uint8_t> & img1 = *seq1.getFrame();
		avlib::CImage<uint8_t> & img2 = *seq2.getFrame();
		PSNRResultItem<float> psnr = Compute(img1, img2);
		results.PSNR += psnr;
		switch(frame_type)
		{
		case avlib::FRAME_TYPE_I:
			results.I_FRAME_PSNR += psnr;
			break;
		case avlib::FRAME_TYPE_P:
			results.P_FRAME_PSNR += psnr;
			break;
		default:
			break;
		}
	}
	return results;
}

PSNRResultItem<float> CPSNR::Compute(avlib::CImage<uint8_t> & img1, avlib::CImage<uint8_t> & img2)
{
	if(img1.getFormat() != img2.getFormat())
	{
		throw utils::StringFormatException("Images formats do not match");
	}
	if(img1.getFormat().Type != avlib::IMAGE_TYPE_YUV420)
	{
		throw utils::StringFormatException("Images have to be in YUV420 format");
	}	
	PSNRResultItem<float> psnr;
	psnr.Y = Compute(img1[0], img2[0]);
	psnr.U = Compute(img1[1], img2[1]);
	psnr.V = Compute(img1[2], img2[2]);
	return psnr;
}

float CPSNR::Compute(avlib::CComponent<uint8_t> & cmp1, avlib::CComponent<uint8_t> & cmp2)
{
	if(cmp1.getSize() != cmp2.getSize())
	{
		throw utils::StringFormatException("Components sizes do not match");
	}
	float sum = 0.0;
	for(int y=0; y<cmp1.getHeight(); y++)
	{
		for(int x=0;x<cmp1.getWidth();x++)
		{
			float v1 = (float)cmp1[y][x];
			float v2 = (float)cmp2[y][x];
			float d = v1-v2;
			float d2 = pow(d, 2);
			sum += d2;
		}
	}
	float mn = (float)cmp1.getHeight()*(float)cmp2.getWidth();
	float mse = sum/mn;
	float psnr = 20*log10(static_cast<double>(255)) - 10*log10(mse);
	return psnr;
}

}
