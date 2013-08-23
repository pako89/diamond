#ifndef _IMAGE_H
#define _IMAGE_H

#include <stdlib.h>
#include <component.h>
#include <avlib.h>

namespace avlib
{

extern const char * ImageTypeString[];

template <class T>
class CImage
{
public:
	CImage();
	CImage(CSize size);
	CImage(CImageFormat format);
	CImage(ImageType type, CSize size);
	CImage(ImageType type, int height, int width);
	CImage(const CImage & src);
	CImage & operator=(const CImage & src);
	template <class U> CImage(const CImage<U> & src);
	template <class U> CImage & operator=(const CImage<U> & src);
	virtual ~CImage();
	virtual int getComponents(void);
	virtual bool setFormat(CImageFormat format);
	virtual CImageFormat getFormat(void);
	virtual CComponent<T> & operator[](int index);
	virtual int getScale(int index);
protected:
	virtual void release();
	virtual void alloc(int num);
	int m_comp_num;
	CComponent<T> ** m_comp;
	int * m_scale;
	CImageFormat m_format;
template <class> friend class CImage;

};

}

#endif //_IMAGE_H
