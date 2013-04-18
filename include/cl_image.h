#ifndef _CL_IMAGE_H
#define _CL_IMAGE_H

#include <utils.h>
#include <image.h>
#include <cl_common.h>
#include <cl_component.h>
#include <cl_image.h>
#include <cl_dct.h>

namespace avlib
{

template <class T>
class CCLImage : public CImage<T>
{
public:
	CCLImage(cl_handle h);
	CCLImage(cl_handle h, CSize size);
	CCLImage(cl_handle h, CImageFormat format);
	CCLImage(cl_handle h, ImageType type, CSize size);
	CCLImage(cl_handle h, ImageType type, int height, int width);
	virtual void CopyToDevice();
	virtual void CopyToHost();
	virtual CCLComponent<T> & getCLComponent(int index);
protected:
	virtual void alloc(int num);
	cl_handle m_clh;
};

}

#endif //_CL_IMAGE_H
