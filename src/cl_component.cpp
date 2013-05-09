#include <cl_component.h>

namespace avlib
{

template <class T>
CCLComponent<T>::CCLComponent(CCLDevice * dev, cl_mem_flags mem_flags) :
	m_dev(dev),
	m_cldata(0),
	m_mem_flags(mem_flags)
{
}

template <class T>
CCLComponent<T>::CCLComponent(CCLDevice * dev, CSize size, cl_mem_flags mem_flags) :
	m_dev(dev),
	m_cldata(0),
	m_mem_flags(mem_flags)
{
	this->setSize(size, mem_flags);
}

template <class T>
CCLComponent<T>::CCLComponent(CCLDevice * dev, int height, int width, cl_mem_flags mem_flags) :
	m_dev(dev),
	CComponent<T>(height, width),
	m_cldata(0),
	m_mem_flags(mem_flags)
{
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
		setSize(size, CL_MEM_READ_WRITE);
	}
	else
	{
		setSize(size, this->m_mem_flags);
	}
}

template <class T>
bool CCLComponent<T>::setSize(CSize size, cl_mem_flags mem_flags)
{
	if(CComponent<T>::setSize(size))
	{
		if(this->m_size != size)
		{
			clReleaseMemObject(this->m_cldata);
		}
		cl_int err;
		this->m_cldata = clCreateBuffer(this->m_dev->getContext(), mem_flags, this->getBytesCount(), NULL, &err);
		if(!this->m_cldata || CL_SUCCESS != err)
		{
			throw utils::StringFormatException("clCreatebuffer(%d)[cl_mem_flags=%x]\n", err, mem_flags);
		}
		this->m_mem_flags = mem_flags;
		return true;
	}
	else
	{
		return false;
	}

}

template <class T>
void CCLComponent<T>::CopyToDevice()
{
	cl_int err;
	err = clEnqueueWriteBuffer(this->m_dev->getCommandQueue(), this->m_cldata, CL_TRUE, 0, this->getBytesCount(), this->m_data, 0, NULL, NULL);
	if(CL_SUCCESS != err)
	{
		throw utils::StringFormatException("clEnqueueWriteBuffer(%d)\n", err);
	}
}

template <class T>
cl_mem CCLComponent<T>::getCLMem()
{
	return this->m_cldata;
}

template <class T>
void CCLComponent<T>::CopyToHost()
{
	cl_int err;
	err = clEnqueueReadBuffer(this->m_dev->getCommandQueue(), this->m_cldata, CL_TRUE, 0, this->getBytesCount(), this->m_data, 0, NULL, NULL);
	if(CL_SUCCESS != err)
	{
		throw utils::StringFormatException("clEnqueueReadBuffer(%d)\n", err);
	}
}

INSTANTIATE(CCLComponent, float);
}