#include <prediction.h>
#include <utils.h>
#include <cmath>
#include <limits>

namespace avlib
{

CPredictionInfo::CPredictionInfo()
{
	m_info.dy = 0;
	m_info.dx = 0;
}

CPredictionInfo::CPredictionInfo(int32_t dy, int32_t dx)
{
	m_info.dy = dy;
	m_info.dx = dx;
}

CPredictionInfo::CPredictionInfo(prediction_info_t info)
{
	m_info.dy = info.dy;
	m_info.dx = info.dx;
}

CPredictionInfo::~CPredictionInfo()
{
}

int32_t CPredictionInfo::deltaX()
{
	return m_info.dx;
}

int32_t CPredictionInfo::deltaY()
{
	return m_info.dy;
}

void CPredictionInfo::Encode(CBitstream * pBstr)
{
	pBstr->putBits(8*sizeof(int32_t), m_info.dy);
	pBstr->putBits(8*sizeof(int32_t), m_info.dx);
}

void CPredictionInfo::Decode(CBitstream * pBstr)
{
	m_info.dy = pBstr->getBits(8*sizeof(int32_t));
	m_info.dx = pBstr->getBits(8*sizeof(int32_t));
}

void CPredictionInfo::Encode(CDynamicHuffman<int32_t> * pHuff, CBitstream * pBstr)
{
	pHuff->Encode(m_info.dy, pBstr);
	pHuff->Encode(m_info.dx, pBstr);
}

void CPredictionInfo::Decode(CDynamicHuffman<int32_t> * pHuff, CBitstream * pBstr)
{
	pHuff->Decode(pBstr, &m_info.dy);
	pHuff->Decode(pBstr, &m_info.dx);
}

CPredictionInfoTable::CPredictionInfoTable() :
	m_size(0, 0),
	m_tab(NULL)
{
}

CPredictionInfoTable::CPredictionInfoTable(CSize size) :
	m_size(size),
	m_tab(NULL)
{
	setSize(size);
}

CPredictionInfoTable::CPredictionInfoTable(const CPredictionInfoTable & src) :
	m_size(0, 0),
	m_tab(NULL)
{
	setSize(src.m_size, false);
	for(int y=0;y<m_size.Height;y++)
	{
		for(int x=0;x<m_size.Width;x++)
		{
			m_tab[y][x] = src.m_tab[y][x];
		}
	}
}

CPredictionInfoTable::~CPredictionInfoTable()
{
	release();
}

void CPredictionInfoTable::release()
{
	if(NULL != m_tab)
	{
		for(int i=0;i<m_size.Height; i++)
		{
			delete [] m_tab[i];
		}
		delete [] m_tab;
		m_tab = NULL;
	}
}

void CPredictionInfoTable::setSize(CSize size, bool copy)
{
	if(size.Width <= 0 || size.Height <= 0)
	{
		throw utils::StringFormatException("wrong CPredictionInfoTable size [Height = %d, Width = %d]", size.Height, size.Width);
	}
	CPredictionInfo ** tab = NULL;
	tab = new CPredictionInfo*[size.Height];
	for(int i=0;i<size.Height;i++)
	{
		tab[i] = new CPredictionInfo[size.Width];
	}
	if(NULL != m_tab)
	{
		if(copy)
		{
			int w = utils::min(size.Width, m_size.Width);
			int h = utils::min(size.Height, m_size.Height);
			for(int y=0;y<h;y++)
			{
				for(int x=0;x<w;x++)
				{
					tab[y][x] = m_tab[y][x];
				}
			}
		}
		release();
	}
	m_tab = tab;
	m_size = size;
}

CPredictionInfo * CPredictionInfoTable::operator[](int index)
{
	if(index < 0 || index >= m_size.Height)
	{
		throw utils::StringFormatException("CPredictionInfoTable::operator[]: wrong index '%d'", index);
	}
	return m_tab[index];
}

CPrediction::CPrediction() : 
	m_last(NULL),
	m_IFT(NULL),
	m_IFIT(NULL),
	m_max(2)
{
}

CPrediction::CPrediction(CImageFormat format) :
	m_IFT(NULL),
	m_IFIT(NULL),
	m_max(2)
{
	m_last = new CImage<float>(format);
}

CPrediction::~CPrediction()
{
	if(NULL != m_last)
	{
		delete m_last;
	}
}

void CPrediction::Transform(CImage<float> * src, CImage<float> * dst, CPredictionInfoTable * pPredInfo, FRAME_TYPE type)
{
	if(FRAME_TYPE_I == type)
	{
		m_IFT->Transform(src, dst);
	}
	else if(FRAME_TYPE_P == type)
	{
		CSize size((*src)[0].getHeight(), (*src)[0].getWidth());
		for(int y=0; y < size.Height; y+=8)
		{
			for(int x=0; x < size.Width; x+=8)
			{
				(*pPredInfo)[y/8][x/8] = predict(&(*src)[0][y][x], CPoint(0, y, x), size);
			}
		}
		for(int k=0;k<src->getComponents();k++)
		{
			CSize size((*src)[k].getHeight(), (*src)[k].getWidth());
			for(int y=0; y < (*src)[k].getHeight(); y+=8)
			{
				for(int x=0; x < (*src)[k].getWidth(); x+=8)
				{
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


CPredictionInfo CPrediction::predict(float * pSrc, CPoint p, CSize s)
{
	int dy = p.Y>8?-1:0;
	int dx = p.X>8?-1:0;
	struct
	{
		CPoint p;
		float d;
	} min = {
		{p.Y, p.X},
		std::numeric_limits<float>::max()
	};
	CPoint dp;
	for(int dy = -m_max ; dy <= m_max ; dy++)
	{
		for(int dx=-m_max ; dx <= m_max ; dx++)
		{
			dp.Y = p.Y + dy*8;
			dp.X = p.X + dx*8;
			if(dp.X >= 0 && dp.Y >= 0 && (dp.Y+8) < s.Height && (dp.X+8) < s.Width)
			{
				float d = diff_mse(pSrc, &(*m_last)[p.Z][dp.Y][dp.X], s);
				if(d < min.d)
				{
					min.d = d;
					min.p = dp;
				}
			}
		}
	}
	//if(min.d != 0) dbg("Min = {dx=%d, dy=%d, d=%f}\n", (min.p.X-p.X)/8, (min.p.Y-p.Y)/8, min.d);
	return CPredictionInfo((min.p.Y-p.Y)/8, (min.p.X-p.X)/8);
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

void CPrediction::TransformBlock(float * pSrc, float * pDst, CPoint p, CSize s, CPredictionInfo predInfo)
{
	int ly = p.Y + predInfo.deltaY()*8;
	int lx = p.X + predInfo.deltaX()*8;
	float * lastPtr = &(*m_last)[p.Z][ly][lx];
	for(int y=0;y<8;y++)
	{
		for(int x=0;x<8;x++)
		{
			pDst[y*s.Width+x] = pSrc[y*s.Width+x] - lastPtr[y*s.Width+x];
		}
	}
}

void CPrediction::ITransformBlock(float * pSrc, float * pDst, CPoint p, CSize s, CPredictionInfo predInfo)
{
	//dbg("predInfo.deltaY()=%d\n", predInfo.deltaY());
	//dbg("predInfo.deltaX()=%d\n", predInfo.deltaX());
	int ly = p.Y + predInfo.deltaY()*8;
	int lx = p.X + predInfo.deltaX()*8;
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

}
