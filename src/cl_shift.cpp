#include <cl_shift.h>

namespace avlib
{

template <class T>
CCLShift<T>::CCLShift(CCLDevice * dev, cl_program program, const char * kernel) :
	ICLKernel(dev, program, kernel)
{
}

template <class T>
CCLShift<T>::CCLShift(T s, CCLDevice * dev, cl_program program, const char * kernel) :
	ICLKernel(dev, program, kernel),
	CShift<T>::CShift(s)
{
}

template <class T>
void CCLShift<T>::doTransform(CImage<T> * src, CImage<T> * dst)
{
	CCLImage<float> * clSrc = dynamic_cast<CCLImage<T>*>(src);
	CCLImage<float> * clDst = dynamic_cast<CCLImage<T>*>(dst);
	if(clSrc && clDst && this->m_kernel)
	{
		for(int i=0;i<clSrc->getComponents(); i++)
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

			SetArg(0, sizeof(srcMem), &srcMem);
			SetArg(1, sizeof(dstMem), &dstMem);
			SetArg(2, sizeof(height), &height);
			SetArg(3, sizeof(width), &width);
			SetArg(4, sizeof(this->m_shift), &this->m_shift);
			EnqueueNDRangeKernel(2, global_work_size, local_work_size, 0, NULL, NULL);
#ifdef CL_FINISH_KERNEL
			Finish();
#endif
		}
	}
	else
	{
		CShift<T>::doTransform(src, dst);
	}
	
}

INSTANTIATE(CCLShift, float);

}
