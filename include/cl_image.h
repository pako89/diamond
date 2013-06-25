#ifndef _CL_IMAGE_H
#define _CL_IMAGE_H

#include <utils.h>
#include <image.h>

#include <cl_device.h>
#include <cl_component.h>
#include <cl_image.h>
#include <cl_dct.h>

namespace avlib
{

template <class T>
class CCLImage : public CImage<T>
{
public:
	CCLImage(CCLDevice * dev);
	CCLImage(CCLDevice * dev, CSize size, cl_mem_flags mem_flags = CL_MEM_READ_WRITE);
	CCLImage(CCLDevice * dev, CImageFormat format, cl_mem_flags mem_flags = CL_MEM_READ_WRITE);
	CCLImage(CCLDevice * dev, ImageType type, CSize size, cl_mem_flags mem_flags = CL_MEM_READ_WRITE);
	CCLImage(CCLDevice * dev, ImageType type, int height, int width, cl_mem_flags mem_flags = CL_MEM_READ_WRITE);
	virtual void CopyToDevice();
	virtual void CopyToHost();
	virtual CCLComponent<T> & getCLComponent(int index);
	virtual bool getAutoCopy();
	virtual void setAutoCopy(bool autoCopy);
protected:
	virtual void alloc(int num);
	CCLDevice * m_dev;
	cl_mem_flags m_mem_flags;
};

}

#endif //_CL_IMAGE_H
