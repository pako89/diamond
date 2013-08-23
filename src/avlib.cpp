#include <avlib.h>
#include <utils.h>
#include <string.h>
#include <string>

namespace avlib
{

CPoint::CPoint() : 
	X(0),
	Y(0),
	Z(0)
{
}

CPoint::CPoint(int y, int x) :
	X(x),
	Y(y),
	Z(0)
{
}

CPoint::CPoint(int z, int y, int x) :
	X(x),
	Y(y),
	Z(z)
{
}

CPoint::~CPoint()
{
}

CSize::CSize() :
	Height(0),
	Width(0)
{
}
	
CSize::CSize(const CSize & size, int align)
{
	setSize(size.Height, size.Width, align);
}

CSize::CSize(int height, int width, int align)
{
	setSize(height, width, align);
}

CSize::~CSize()
{
}

void CSize::setSize(int height, int width, int align)
{
	if(align)
	{
		if(height%align)
		{
			this->Height = height + align-(height%align);
		}
		else
		{
			this->Height = height;
		}
		if(width%align)
		{
			this->Width = width + align-(width%align);
		}
		else
		{
			this->Width = width;
		}
	}
	else
	{
		this->Height = height;
		this->Width = width;
	}
}

bool CSize::operator==(int val)
{
	return (Height == val && Width == val);
}

bool CSize::operator==(const CSize src)
{
	return (Height == src.Height && Width == src.Width);
}

bool CSize::operator!=(int val)
{
	return !operator==(val);
}

bool CSize::operator!=(const CSize src)
{
	return !operator==(src);
}

ImageType CImageFormat::ParseImageType(std::string str)
{
	if(str == "YUV420")
	{
		return avlib::IMAGE_TYPE_YUV420;
	}
	else if(str == "420jpeg")
	{
		return avlib::IMAGE_TYPE_YUV420;
	}
	else
	{
		return avlib::IMAGE_TYPE_UNKNOWN;
	}
}

CImageFormat::CImageFormat() :
	Type(IMAGE_TYPE_UNKNOWN),
	Size(0,0)
{
}

CImageFormat::CImageFormat(ImageType type, CSize size, int align) :
	Type(type),
	Size(size),
	Align(align)
{
}

CImageFormat::CImageFormat(ImageType type, int height, int width, int align) :
	Type(type),
	Size(height, width),
	Align(align)
{
}

CImageFormat::~CImageFormat()
{
}

bool CImageFormat::operator==(const CImageFormat img)
{
	return (Type == img.Type && Size == img.Size);
}

bool CImageFormat::operator!=(const CImageFormat img)
{
	return !operator==(img);
}
	
CFrameRate CFrameRate::ParseFrameRate(std::string s)
{
	int pos = s.find(':');
	std::string nom = std::string(s, 0, pos);
	std::string denom = std::string(s, pos+1, s.size());
	CFrameRate ret;
	ret.Nom = utils::ParseInt(nom);
	ret.Denom = utils::ParseInt(denom);
	return ret;
}

CFrameRate::CFrameRate()
{
}

CFrameRate::CFrameRate(uint32_t nom, uint32_t denom):
	Nom(nom),
	Denom(denom)
{
}

CFrameRate::~CFrameRate()
{
}
	
}
