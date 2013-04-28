#ifndef _CL_COMPONENT_H
#define _CL_COMPONENT_H

#include <component.h>
#include <utils.h>

#include <cl_device.h>
#include <CL/opencl.h>

namespace avlib
{

template <class T>
class CCLComponent : public CComponent<T>
{
public:
	CCLComponent(CCLDevice * dev, cl_mem_flags mem_flags = CL_MEM_READ_WRITE);
	CCLComponent(CCLDevice * dev, CSize size, cl_mem_flags mem_flags = CL_MEM_READ_WRITE);
	CCLComponent(CCLDevice * dev, int height, int width, cl_mem_flags mem_flags = CL_MEM_READ_WRITE);
	virtual ~CCLComponent();
	virtual bool setSize(CSize size);
	virtual bool setSize(CSize size, cl_mem_flags mem_flags);
	virtual void CopyToDevice();
	virtual void CopyToHost();
	virtual cl_mem getCLMem();
protected:
	CCLDevice * m_dev;
	cl_mem m_cldata;
	cl_mem_flags m_mem_flags;
};

}

#endif //_CL_COMPONENT_H
