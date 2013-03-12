#ifndef _IMAGE_H
#define _IMAGE_H

#include <stdlib.h>
#include <component.h>

namespace avlib
{

enum ImageType
{
	IMAGE_YUV420 = 0,
	IMAGE_RGB,
	IMAGE_ARGB,
};

template <class T>
class CImage
{
public:
	CImage();
	CImage(enum ImageType type, int height, int width);
	virtual ~CImage();
protected:
	CComponent<T> * m_comp;

};

}

#endif //_IMAGE_H