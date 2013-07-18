#ifndef _CL_COMPONENT_H
#define _CL_COMPONENT_H

#include <component.h>
#include <utils.h>
#include <mtimer.h>
#include <cl_device.h>

namespace avlib
{

template <class T>
class CCLComponent : public CComponent<T>
{
public:
	CCLComponent(CCLDevice * dev, cl_mem_flags mem_flags = CL_MEM_READ_WRITE);
	CCLComponent(CCLDevice * dev, CSize size, cl_mem_flags mem_flags = CL_MEM_READ_WRITE);
	virtual ~CCLComponent();
	virtual T * operator[](int height);
	virtual bool setSize(CSize size);
	virtual bool setSize(CSize size, cl_mem_flags mem_flags);
	virtual void CopyToDevice();
	virtual void CopyToHost();
	virtual cl_mem getCLMem(bool dosync=true);
	virtual bool getAutoCopy();
	virtual void setAutoCopy(bool autoCopy);
protected:
	virtual void copy(const CComponent<T> * src);
	enum CLMEM_STATE
	{
		CLMEM_STATE_HOST=0,
		CLMEM_STATE_DEVICE
	};
	void sync(enum CLMEM_STATE nextState);
	CCLDevice * m_dev;
	cl_mem m_cldata;
	cl_mem_flags m_mem_flags;
	bool m_autoCopy;
	enum CLMEM_STATE m_state;
};

}

#endif //_CL_COMPONENT_H
