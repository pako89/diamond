#ifndef _IMAGE_H
#define _IMAGE_H

#include <stdlib.h>
#include <component.h>

namespace avlib
{

enum ImageType
{
	IMAGE_TYPE_UNKNOWN = 0,
	IMAGE_TYPE_YUV420,
	IMAGE_TYPE_RGB,
	IMAGE_TYPE_ARGB,
};

extern const char * ImageTypeString[];

template <class T>
class CImage
{
public:
	CImage();
	CImage(enum ImageType type, int height, int width);
	virtual ~CImage();
	virtual int getComponents(void);
	virtual bool setSize(enum ImageType type, int height, int width);
	virtual CComponent<T> & operator[](int index);
protected:
	int m_comp_num;
	CComponent<T> * m_comp;

};

}

#endif //_IMAGE_H
