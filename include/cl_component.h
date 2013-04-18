#ifndef _CL_COMPONENT_H
#define _CL_COMPONENT_H

#include <component.h>
#include <utils.h>
#include <cl_common.h>
#include <CL/opencl.h>

namespace avlib
{

template <class T>
class CCLComponent : public CComponent<T>
{
public:
	CCLComponent(cl_handle h);
	CCLComponent(cl_handle h, CSize size);
	CCLComponent(cl_handle h, int height, int width);
	virtual ~CCLComponent();
	virtual bool setSize(CSize size);
	virtual void setHandle(cl_handle h);
	virtual void CopyToDevice();
	virtual void CopyToHost();
	virtual cl_mem getCLMem();
protected:
	cl_handle m_clh;
	cl_mem m_cldata;
};

}

#endif //_CL_COMPONENT_H
