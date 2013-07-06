#include <cl_interpolation.h>
#include <cl_image.h>
#include <utils.h>

namespace avlib
{

template <class T>
CCLInterpolation<T>::CCLInterpolation(CCLDevice * dev, cl_program program, const char * kernel)
{
	m_kernel = new ICLKernel(dev, program, kernel);
}

template <class T>
CCLInterpolation<T>::~CCLInterpolation()
{
	RELEASE(m_kernel);
}

template <class T>
void CCLInterpolation<T>::doTransform(CImage<T> * src, CImage<T> * dst)
{
	CCLImage<T> * clSrc = dynamic_cast<CCLImage<T>*>(src);
	CCLImage<T> * clDst = dynamic_cast<CCLImage<T>*>(dst);
	if(NULL != m_kernel && NULL != clSrc && NULL != clDst)
	{
		for(int i=0;i<clSrc->getComponents();i++)
		{
			int srcHeight = (*clSrc)[i].getSize().Height;
			int srcWidth = (*clSrc)[i].getSize().Width;
			int dstHeight = (*clDst)[i].getSize().Height;
			int dstWidth = (*clDst)[i].getSize().Width;
			cl_mem srcMem = clSrc->getCLComponent(i).getCLMem(true);
			cl_mem dstMem = clDst->getCLComponent(i).getCLMem(false);
			size_t global_work_size[2];
			global_work_size[0] = dstHeight;
			global_work_size[1] = dstWidth;
			size_t local_work_size[2];
			local_work_size[0] = 8;
			local_work_size[1] = 8;

			m_kernel->SetArg(0, sizeof(srcMem), &srcMem);
			m_kernel->SetArg(1, sizeof(srcHeight), &srcHeight);
			m_kernel->SetArg(2, sizeof(srcWidth), &srcWidth);
			m_kernel->SetArg(3, sizeof(dstMem), &dstMem);
			m_kernel->SetArg(4, sizeof(dstHeight), &dstHeight);
			m_kernel->SetArg(5, sizeof(dstWidth), &dstWidth);
			m_kernel->SetArg(6, sizeof(this->m_scale), &this->m_scale);
			m_kernel->EnqueueNDRangeKernel(2, global_work_size, local_work_size, 0, NULL, NULL);
#ifdef CL_KERNEL_FINISH
			m_kernel->Finish();
#endif
			
		}
	}
	else
	{
		CInterpolation<T>::doTransform(src, dst);
	}
}

INSTANTIATE(CCLInterpolation, float);
INSTANTIATE(CCLInterpolation, int16_t);

}
