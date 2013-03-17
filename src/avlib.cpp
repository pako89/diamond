#include <avlib.h>

namespace avlib
{

CSize::CSize() :
	Height(0),
	Width(0)
{
}

CSize::CSize(int height, int width) : 
	Height(height),
	Width(width)
{
}

CSize::~CSize()
{
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

CImageFormat::CImageFormat() :
	Type(IMAGE_TYPE_UNKNOWN),
	Size(0,0)
{
}

CImageFormat::CImageFormat(ImageType type, CSize size) :
	Type(type),
	Size(size)
{
}

CImageFormat::CImageFormat(ImageType type, int height, int width) :
	Type(type),
	Size(height, width)
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

}
