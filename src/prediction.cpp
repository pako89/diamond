#include <prediction.h>
#include <utils.h>
#include <cmath>
#include <limits>

#ifndef DEFAULT_MAX_PREDICTION
#define DEFAULT_MAX_PREDICTION	2
#endif

namespace avlib
{

CPrediction::CPrediction() : 
	m_last(NULL),
	m_IFT(NULL),
	m_IFIT(NULL),
	m_max(DEFAULT_MAX_PREDICTION)
{
	m_huff = new CDynamicHuffman<int>();
}

CPrediction::CPrediction(CImageFormat format) :
	m_IFT(NULL),
	m_IFIT(NULL),
	m_max(DEFAULT_MAX_PREDICTION)
{
	m_last = new CImage<float>(format);
	m_huff = new CDynamicHuffman<int>();
}

CPrediction::~CPrediction()
{
	RELEASE(m_last);
	RELEASE(m_huff);
}

void CPrediction::Transform(CImage<float> * src, CImage<float> * dst, CPredictionInfoTable * pPredInfo, FRAME_TYPE type)
{
	if(FRAME_TYPE_I == type)
	{
		m_IFT->Transform(src, dst);
	}
	else if(FRAME_TYPE_P == type)
	{
		for(int k=0;k<src->getComponents();k++)
		{
			CSize size((*src)[k].getHeight(), (*src)[k].getWidth());
			for(int y=0; y < size.Height; y+=8)
			{
				for(int x=0; x < size.Width; x+=8)
				{
					if(k==0) (*pPredInfo)[y/8][x/8] = predict(&(*src)[0][y][x], CPoint(0, y, x), size);
					TransformBlock(&(*src)[k][y][x], &(*dst)[k][y][x], CPoint(k, y, x), size, (*pPredInfo)[y/8][x/8]);
				}
			}
		}
	}
	else
	{
		throw utils::StringFormatException("Invalid FRAME TYPE (%d)\n", type);
	}
}


prediction_info_t CPrediction::predict(float * pSrc, CPoint p, CSize s)
{
	int dy = p.Y>8?-1:0;
	int dx = p.X>8?-1:0;
	struct
	{
		CPoint p;
		float d;
	} min;
	min.p = p;
	min.d = std::numeric_limits<float>::max();
	CPoint dp;
	for(int dy = -m_max ; dy <= m_max ; dy++)
	{
		for(int dx=-m_max ; dx <= m_max ; dx++)
		{
			dp.Y = p.Y + dy*8;
			dp.X = p.X + dx*8;
			if(dp.X >= 0 && dp.Y >= 0 && (dp.Y+8) < s.Height && (dp.X+8) < s.Width)
			{
				float d;
#if DEFAULT_PREDICTION_METHOD == PREDICTION_METHOD_MSE				
				d = diff_mse(pSrc, &(*m_last)[p.Z][dp.Y][dp.X], s);
#elif DEFAULT_PREDICTION_METHOD == PREDICTION_METHOD_ABS
				d = diff_abs(pSrc, &(*m_last)[p.Z][dp.Y][dp.X], s);
#else
#error Unknown prediction method 
#endif
				if(d < min.d)
				{
					min.d = d;
					min.p = dp;
				}
			}
		}
	}
	prediction_info_t info;
	info.dy = (min.p.Y-p.Y)/8;
	info.dx = (min.p.X-p.X)/8;
	return info;
}

float CPrediction::diff_abs(float * src, float * ref, CSize s)
{
	float ret = 0.0;
	for(int y=0;y<8;y++)
	{
		for(int x=0;x<8;x++)
		{
			ret += fabs(src[y*s.Width+x] - ref[y*s.Width+x]);
		}
	}
	return ret;
}

float CPrediction::diff_mse(float * src, float * ref, CSize s)
{
	float ret = 0.0;
	for(int y=0;y<8;y++)
	{
		for(int x=0;x<8;x++)
		{
			ret += pow(src[y*s.Width+x] - ref[y*s.Width+x], 2);
		}
	}
	return sqrt(ret);

}

void CPrediction::ITransform(CImage<float> * src, CImage<float> * dst, CPredictionInfoTable * pPredInfo, FRAME_TYPE type)
{
	if(src->getFormat() != dst->getFormat())
	{
		throw utils::StringFormatException("formats does not match\n");
	}
	if(FRAME_TYPE_I == type)
	{
		m_IFIT->Transform(src, dst);
	}
	else if(FRAME_TYPE_P == type)
	{
		for(int k=0;k<src->getComponents();k++)
		{
			CSize size((*src)[k].getHeight(), (*src)[k].getWidth());
			for(int y=0; y < (*src)[k].getHeight(); y+=8)
			{
				for(int x=0; x < (*src)[k].getWidth(); x+=8)
				{
					ITransformBlock(&(*src)[k][y][x], &(*dst)[k][y][x], CPoint(k, y, x), size, (*pPredInfo)[y/8][x/8]);
				}
			}
		}
	}
	else
	{
		throw utils::StringFormatException("CImage::operator- not implemented");
	}
	*m_last = *dst;
}

void CPrediction::TransformBlock(float * pSrc, float * pDst, CPoint p, CSize s, prediction_info_t predInfo)
{
	int ly = p.Y + predInfo.dy*8;
	int lx = p.X + predInfo.dx*8;
	float * lastPtr = &(*m_last)[p.Z][ly][lx];
	for(int y=0;y<8;y++)
	{
		for(int x=0;x<8;x++)
		{
			pDst[y*s.Width+x] = pSrc[y*s.Width+x] - lastPtr[y*s.Width+x];
		}
	}
}

void CPrediction::ITransformBlock(float * pSrc, float * pDst, CPoint p, CSize s, prediction_info_t predInfo)
{
	int ly = p.Y + predInfo.dy*8;
	int lx = p.X + predInfo.dx*8;
	float * lastPtr = &(*m_last)[p.Z][ly][lx];
	for(int y=0;y<8;y++)
	{
		for(int x=0;x<8;x++)
		{
			pDst[y*s.Width+x] = pSrc[y*s.Width+x] + lastPtr[y*s.Width+x];
		}
	}
}

void CPrediction::setIFrameTransform(CTransform<float, float> * t)
{
	if(t != NULL)
	{
		m_IFT = t;
	}
	else
	{
		throw utils::StringFormatException("Invalid I Frame transform");
	}
}

void CPrediction::setIFrameITransform(CTransform<float, float> * t)
{
	if(t != NULL)
	{
		m_IFIT = t;
	}
	else
	{
		throw utils::StringFormatException("Invalid I Frame inverse transform");
	}
}

void CPrediction::Encode(CPredictionInfoTable * pPred, CBitstream * pBstr)
{
	for(int y=0;y<pPred->getHeight();y++)
	{
		for(int x=0;x<pPred->getWidth();x++)
		{
			encodePredictionInfo((*pPred)[y][x], pBstr);
		}
	}
}

void CPrediction::Decode(CPredictionInfoTable * pPred, CBitstream * pBstr)
{
	for(int y=0;y<pPred->getHeight();y++)
	{
		for(int x=0;x<pPred->getWidth();x++)
		{
			(*pPred)[y][x] = decodePredictionInfo(pBstr);
		}
	}
}

void CPrediction::encodePredictionInfo(prediction_info_t info, CBitstream * pBstr)
{
	m_huff->Encode(info.dy, pBstr);
	m_huff->Encode(info.dx, pBstr);
}

prediction_info_t CPrediction::decodePredictionInfo(CBitstream * pBstr)
{
	prediction_info_t info;
	m_huff->Decode(pBstr, &info.dy);
	m_huff->Decode(pBstr, &info.dx);
	return info;
}

}
