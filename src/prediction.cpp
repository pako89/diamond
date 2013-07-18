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
	m_interpol(NULL),
	m_last(NULL),
	m_IFT(NULL),
	m_IFIT(NULL),
	m_max(DEFAULT_MAX_PREDICTION),
	m_huff(NULL)
{
	m_huff = new CDynamicHuffman<int>();
}

CPrediction::~CPrediction()
{
	RELEASE(m_last);
	RELEASE(m_huff);
	RELEASE(m_interpol);
}

void CPrediction::Init(CImageFormat format, int scale)
{
	if(NULL == m_interpol)
	{
		m_interpol = new CInterpolation<float>();
		m_interpol->setScale(scale);
	}
	if(NULL == m_last)
	{
		CImageFormat newFormat = format;
		newFormat.Size.Width *= m_interpol->getScale();
		newFormat.Size.Height *= m_interpol->getScale();
		m_last = new CImage<float>(newFormat);
	}
}

utils::CTimer CPrediction::getTimer(PredictionTimer timer)
{
	switch(timer)
	{
	case PredictionTimer_Transform:
		return m_timerTransform;
	case PredictionTimer_ITransform:
		return m_timerITransform;
	case PredictionTimer_Prediction:
		return m_timerPrediction;
	case PredictionTimer_EncodePrediction:
		return m_timerEncodePrediction;
	case PredictionTimer_Interpolation:
		return m_timerInterpolation;
	case PredictionTimer_CopyLast:
		return m_timerCopyLast;
	default:
		return utils::CTimer();
	}
}

int CPrediction::getInterpolationScale()
{
	return m_interpol->getScale();
}

void CPrediction::doPredict(CComponent<float> * pSrc, CPredictionInfoTable * pPred)
{
	CSize size = pSrc->getSize();
	for(int y=0; y < size.Height; y+=16)
	{
		for(int x=0; x < size.Width; x+=16)
		{
			(*pPred)[y/16][x/16] = predict(&(*pSrc)[y][x], CPoint(0, y, x), size);
		}
	}
}

prediction_info_t CPrediction::predict(float * pSrc, CPoint p, CSize srcSize)
{
	prediction_info_t info;
	info.dy = 0;
	info.dx = 0;
	CSize s = (*m_last)[p.Z].getSize();
	int scale = getInterpolationScale();
	p.Y *= scale;
	p.X *= scale;
	if(m_max)
	{
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
				dp.Y = p.Y + dy*16;
				dp.X = p.X + dx*16;
				if(dp.X >= 0 && dp.Y >= 0 && (dp.Y+16) < s.Height && (dp.X+16) < s.Width)
				{
					float d;
#if DEFAULT_PREDICTION_METHOD == PREDICTION_METHOD_MSE				
					d = diff_mse(pSrc, srcSize, &(*m_last)[p.Z][dp.Y][dp.X], s);
#elif DEFAULT_PREDICTION_METHOD == PREDICTION_METHOD_ABS
					d = diff_abs(pSrc, srcSize, &(*m_last)[p.Z][dp.Y][dp.X], s);
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
		info.dy = (min.p.Y-p.Y)/16;
		info.dx = (min.p.X-p.X)/16;
	}
	return info;
}

float CPrediction::diff_abs(float * src, CSize srcSize, float * ref, CSize refSize)
{
	float ret = 0.0;
	for(int y=0;y<16;y++)
	{
		for(int x=0;x<16;x++)
		{
			ret += fabs(src[y*srcSize.Width+x] - ref[y*refSize.Width+x]);
		}
	}
	return ret;
}

float CPrediction::diff_mse(float * src, CSize srcSize, float * ref, CSize refSize)
{
	float ret = 0.0;
	for(int y=0;y<16;y++)
	{
		for(int x=0;x<16;x++)
		{
			ret += pow(src[y*srcSize.Width+x] - ref[y*refSize.Width+x], 2);
		}
	}
	return sqrt(ret);

}

void CPrediction::Transform(CImage<float> * pSrc, CImage<float> * pDst, CPredictionInfoTable * pPredInfo, FRAME_TYPE type)
{
	if(FRAME_TYPE_I == type)
	{
		m_IFT->Transform(pSrc, pDst);
	}
	else if(FRAME_TYPE_P == type)
	{
		m_timerPrediction.start();
		doPredict(&(*pSrc)[0], pPredInfo);
		m_timerPrediction.stop();
		m_timerTransform.start();
		doTransformPFrame(pSrc, pDst, pPredInfo);
		m_timerTransform.stop();
	}
	else
	{
		throw utils::StringFormatException("Invalid FRAME TYPE (%d)\n", type);
	}
}


void CPrediction::ITransform(CImage<float> * pSrc, CImage<float> * pDst, CPredictionInfoTable * pPredInfo, FRAME_TYPE type)
{
	m_timerITransform.start();
	if(pSrc->getFormat() != pDst->getFormat())
	{
		throw utils::StringFormatException("formats do not match\n");
	}
	if(FRAME_TYPE_I == type)
	{
		m_IFIT->Transform(pSrc, pDst);
	}
	else if(FRAME_TYPE_P == type)
	{
		doITransformPFrame(pSrc, pDst, pPredInfo);
	}
	else
	{
		throw utils::StringFormatException("Unknown FRAME_TYPE");
	}
	m_timerITransform.stop();
	if(getInterpolationScale() > 1)
	{
		m_timerInterpolation.start();
		m_interpol->Transform(pDst, m_last);
		m_timerInterpolation.stop();
	}
	else
	{
		m_timerCopyLast.start();
		*m_last = *pDst;
		m_timerCopyLast.stop();
	}
}

void CPrediction::doTransformPFrame(CImage<float> * pSrc, CImage<float> * pDst, CPredictionInfoTable * pPred)
{
	for(int k=0;k<pSrc->getComponents();k++)
	{
		int scale = 16/pSrc->getScale(k);
		CSize size = (*pSrc)[k].getSize();
		for(int y=0; y < size.Height; y+=8)
		{
			for(int x=0; x < size.Width; x+=8)
			{
				prediction_info_t info = (*pPred)[y/scale][x/scale];
				TransformBlock(&(*pSrc)[k][y][x], &(*pDst)[k][y][x], CPoint(k, y, x), size, info, scale);
			}
		}
	}
}

void CPrediction::doITransformPFrame(CImage<float> * pSrc, CImage<float> * pDst, CPredictionInfoTable * pPred)
{
	for(int k=0;k<pSrc->getComponents();k++)
	{
		int scale = 16/pSrc->getScale(k);
		CSize size = (*pSrc)[k].getSize();
		for(int y=0; y < (*pSrc)[k].getHeight(); y+=8)
		{
			for(int x=0; x < (*pSrc)[k].getWidth(); x+=8)
			{
				prediction_info_t info = (*pPred)[y/scale][x/scale];
				ITransformBlock(&(*pSrc)[k][y][x], &(*pDst)[k][y][x], CPoint(k, y, x), size, info, scale);
			}
		}
	}
}

void CPrediction::TransformBlock(float * pSrc, float * pDst, CPoint p, CSize s, prediction_info_t predInfo, int cscale)
{
	int iscale = getInterpolationScale();
	for(int y=0;y<8;y++)
	{
		for(int x=0;x<8;x++)
		{
			int ly = (p.Y+y)*iscale + predInfo.dy*cscale;
			int lx = (p.X+x)*iscale + predInfo.dx*cscale;
			pDst[y*s.Width+x] = pSrc[y*s.Width+x] - (*m_last)[p.Z][ly][lx];
		}
	}
}

void CPrediction::ITransformBlock(float * pSrc, float * pDst, CPoint p, CSize s, prediction_info_t predInfo, int cscale)
{
	int iscale = getInterpolationScale();
	for(int y=0;y<8;y++)
	{
		for(int x=0;x<8;x++)
		{
			int ly = (p.Y+y)*iscale + predInfo.dy*cscale;
			int lx = (p.X+x)*iscale + predInfo.dx*cscale;
			pDst[y*s.Width+x] = pSrc[y*s.Width+x] + (*m_last)[p.Z][ly][lx];
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

void CPrediction::Encode(CPredictionInfoTable * pPred, CBitstream * pBstr, FRAME_TYPE frame_type)
{
	m_timerEncodePrediction.start();
	if(FRAME_TYPE_I != frame_type)
	{
		for(int y=0;y<pPred->getHeight();y++)
		{
			for(int x=0;x<pPred->getWidth();x++)
			{
				encodePredictionInfo((*pPred)[y][x], pBstr);
			}
		}
	}
	m_timerEncodePrediction.stop();
}

void CPrediction::Decode(CPredictionInfoTable * pPred, CBitstream * pBstr, FRAME_TYPE frame_type)
{
	if(FRAME_TYPE_I != frame_type)
	{
		for(int y=0;y<pPred->getHeight();y++)
		{
			for(int x=0;x<pPred->getWidth();x++)
			{
				(*pPred)[y][x] = decodePredictionInfo(pBstr);
			}
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
