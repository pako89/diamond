#ifndef _AVLIB_H
#define _AVLIB_H

namespace avlib
{

class CPoint
{
public:
	CPoint();
	CPoint(int y, int x);
	CPoint(int z, int y, int x);
	~CPoint();
	int X;
	int Y;
	int Z;
};

class CSize
{
public:
	CSize();
	CSize(int height, int width);
	~CSize();
	bool operator==(int val);
	bool operator==(const CSize src);
	bool operator!=(int val);
	bool operator!=(const CSize src);
	int Height;
	int Width;
};

enum ImageType
{
	IMAGE_TYPE_UNKNOWN = 0,
	IMAGE_TYPE_YUV420,
	IMAGE_TYPE_RGB,
	IMAGE_TYPE_ARGB,
};


class CImageFormat
{
public:
	CImageFormat();
	CImageFormat(ImageType type, CSize size);
	CImageFormat(ImageType type, int height, int width);
	~CImageFormat();
	bool operator==(const CImageFormat img);
	bool operator!=(const CImageFormat img);
	ImageType Type;
	CSize Size;
};

}

#endif //_AVLIB_H
