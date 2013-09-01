#include <cl_component.h>
#include <clkernel.h>
#include <cl_timers.h>

namespace avlib
{

template <class T>
CCLComponent<T>::CCLComponent(CCLDevice * dev, cl_mem_flags mem_flags) :
	m_dev(dev),
	m_cldata(0),
	m_mem_flags(mem_flags),
	m_autoCopy(true),
	m_state(CLMEM_STATE_HOST)
{
}

template <class T>
CCLComponent<T>::CCLComponent(CCLDevice * dev, CSize size, cl_mem_flags mem_flags) :
	m_dev(dev),
	m_cldata(0),
	m_mem_flags(mem_flags),
	m_autoCopy(true),
	m_state(CLMEM_STATE_HOST)
{
	this->setSize(size, mem_flags);
}

template <class T>
CCLComponent<T>::~CCLComponent()
{
	if(this->m_cldata)
	{
		clReleaseMemObject(this->m_cldata);
	}
}

template <class T>
bool CCLComponent<T>::setSize(CSize size)
{
	if(!this->m_mem_flags)
	{
		return setSize(size, CL_MEM_READ_WRITE);
	}
	else
	{
		return setSize(size, this->m_mem_flags);
	}
}

template <class T>
bool CCLComponent<T>::setSize(CSize size, cl_mem_flags mem_flags)
{
	if(CComponent<T>::setSize(size))
	{
		if(this->m_size != size || this->m_mem_flags != mem_flags)
		{
			clReleaseMemObject(this->m_cldata);
			this->m_cldata = NULL;
		}
		if(!this->m_cldata)
		{
			cl_int err;
			this->m_cldata = clCreateBuffer(this->m_dev->getContext(), mem_flags, this->getBytesCount(), NULL, &err);
			if(!this->m_cldata || CL_SUCCESS != err)
			{
				throw utils::StringFormatException("clCreatebuffer(%d)[cl_mem_flags=%x]\n", err, mem_flags);
			}
		}		
		return true;
	}
	else
	{
		return false;
	}
}
template <class T>
bool CCLComponent<T>::alloc()
{
	CComponent<T>::alloc();
	if(this->m_cldata)
	{
		clReleaseMemObject(this->m_cldata);
	}
	cl_int err;
	this->m_cldata = clCreateBuffer(this->m_dev->getContext(), m_mem_flags, this->getBytesCount(), NULL, &err);
	if(!this->m_cldata || CL_SUCCESS != err)
	{
		throw utils::StringFormatException("clCreatebuffer(%d)[cl_mem_flags=%x]\n", err, m_mem_flags);
	}
	return true;
}

template <class T>
void CCLComponent<T>::sync(CLMEM_STATE nextState)
{
	if(m_autoCopy && m_state != nextState)
	{
		if(CLMEM_STATE_HOST==m_state &&
		   CLMEM_STATE_DEVICE==nextState)
		{
			CopyToDevice();
		}
		else if(CLMEM_STATE_DEVICE==m_state &&
			CLMEM_STATE_HOST==nextState)
		{
			CopyToHost();
		}
	}
}

template <class T>
cl_mem CCLComponent<T>::getCLMem(bool dosync)
{
	if(dosync)
	{
		sync(CLMEM_STATE_DEVICE);
	}
	else
	{
		m_state = CLMEM_STATE_DEVICE;
	}
	return this->m_cldata;
}

template <class T>
T * CCLComponent<T>::operator[](int h)
{
	sync(CLMEM_STATE_HOST);
	return &this->m_data[h * this->m_size.Width];
}

template <class T>
void CCLComponent<T>::copy(const CComponent<T> * src)
{
	const CCLComponent<T> * clsrc = dynamic_cast<const CCLComponent<T>*>(src);
	if(NULL != clsrc)
	{
		m_dev = clsrc->m_dev;
		m_mem_flags = clsrc->m_mem_flags;
		CComponent<T>::copy(src);
		if(CLMEM_STATE_DEVICE==clsrc->m_state)
		{
			CCLTimers::getCopyBuffer().start();
			cl_int err = clEnqueueCopyBuffer(
					this->m_dev->getCommandQueue(), 
					clsrc->m_cldata, 
					this->m_cldata, 
					0, 
					0, 
					this->getBytesCount(), 
					0, NULL, NULL);
			if(CL_SUCCESS != err)
			{
				throw utils::StringFormatException("clEnqueueCopyBuffer(%d)\n", err);
			}
			m_state = CLMEM_STATE_DEVICE;
			CCLTimers::getCopyBuffer().stop();
		}
	}
	else
	{
		CComponent<T>::copy(src);
	}
}

template <class T>
void CCLComponent<T>::CopyToHost()
{
	cl_int err;
	this->m_dev->Finish();
	CCLTimers::getCopyToHost().start();
	err = clEnqueueReadBuffer(this->m_dev->getCommandQueue(), this->m_cldata, CL_TRUE, 0, this->getBytesCount(), this->m_data, 0, NULL, NULL);
	if(CL_SUCCESS != err)
	{
		throw utils::StringFormatException("clEnqueueReadBuffer(%d)\n", err);
	}
	CCLTimers::getCopyToHost().stop();
	m_state = CLMEM_STATE_HOST;
}

template <class T>
void CCLComponent<T>::CopyToDevice()
{
	cl_int err;
	this->m_dev->Finish();
	CCLTimers::getCopyToDevice().start();
	err = clEnqueueWriteBuffer(this->m_dev->getCommandQueue(), this->m_cldata, CL_TRUE, 0, this->getBytesCount(), this->m_data, 0, NULL, NULL);
	if(CL_SUCCESS != err)
	{
		throw utils::StringFormatException("clEnqueueWriteBuffer(%d)\n", err);
	}
	CCLTimers::getCopyToDevice().stop();
	m_state = CLMEM_STATE_DEVICE;
}

template <class T>
bool CCLComponent<T>::getAutoCopy()
{
	return m_autoCopy;
}

template <class T>
void CCLComponent<T>::setAutoCopy(bool autoCopy)
{
	m_autoCopy = autoCopy;
}

INSTANTIATE(CCLComponent, prediction_info_t);
INSTANTIATE(CCLComponent, float);
INSTANTIATE(CCLComponent, int16_t);
INSTANTIATE(CCLComponent, int);
}
