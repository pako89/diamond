#include <cl_zigzag.h>

namespace avlib
{

template <class S, class D>
CCLZigZag<S, D>::CCLZigZag(CCLDevice * dev, cl_program program, const char * kernel) :
	m_kernel(NULL)
{
	m_kernel = new CCLKernel(dev, program, kernel);
}

template <class S, class D>
void CCLZigZag<S, D>::doTransform(CImage<S> * src, CImage<D> * dst)
{
	CCLImage<S> * clSrc = dynamic_cast<CCLImage<S>*>(src);
	CCLImage<D> * clDst = dynamic_cast<CCLImage<D>*>(dst);
	if(m_kernel && NULL != clSrc && NULL != clDst)
	{
		for(int i = 0 ; i < clSrc->getComponents(); i++)
		{
			int height = (*clSrc)[i].getHeight();
			int width = (*clSrc)[i].getWidth();
			cl_mem srcMem = clSrc->getCLComponent(i).getCLMem(true);
			cl_mem dstMem = clDst->getCLComponent(i).getCLMem(false);
			size_t global_work_size[2];
			global_work_size[0] = height;
			global_work_size[1] = width;
			size_t local_work_size[2];
			local_work_size[0] = 8;
			local_work_size[1] = 8;
			
			m_kernel->SetArg(0, sizeof(srcMem), &srcMem);
			m_kernel->SetArg(1, sizeof(dstMem), &dstMem);
			m_kernel->SetArg(2, sizeof(height), &height);
			m_kernel->SetArg(3, sizeof(width), &width);
			m_kernel->EnqueueNDRangeKernel(2, global_work_size, local_work_size, 0, NULL, NULL);
#ifdef CL_KERNEL_FINISH			
			m_kernel->Finish();
#endif
		}
	}
	else
	{
		CZigZag<S, D>::doTransform(src, dst);
	}
}


INSTANTIATE2(CCLZigZag, float, int16_t);
INSTANTIATE2(CCLZigZag, float, int32_t);
}
