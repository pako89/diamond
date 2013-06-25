#include <cl_dct.h>


namespace avlib
{

CCLDCT::CCLDCT(CCLDevice * dev, cl_program program, const char * kernel) :
	ICLKernel(dev, program, kernel)
{
}

CCLDCT::~CCLDCT()
{

}

void CCLDCT::doTransform(CImage<float> * src, CImage<float> * dst)
{
	CCLImage<float> * clSrc = dynamic_cast<CCLImage<float>*>(src);
	CCLImage<float> * clDst = dynamic_cast<CCLImage<float>*>(dst);
	if(NULL == clSrc || NULL == clDst)
	{
		throw utils::StringFormatException("src or dst is not CCLImage<float>\n");
	}
	cl_int err = 0;
	if(m_kernel)
	{
		for(int i=0;i<clSrc->getComponents(); i++)
		{
			int height = (*clSrc)[i].getHeight();
			int width = (*clSrc)[i].getWidth();
			cl_mem srcMem = clSrc->getCLComponent(i).getCLMem(true);
			cl_mem dstMem = clDst->getCLComponent(i).getCLMem(false);
			size_t global_work_size[2];
			global_work_size[0] = height;
			global_work_size[1] = width/8;
			size_t local_work_size[2];
			local_work_size[0] = 8;
			local_work_size[1] = 1;
			
			SetArg(0, sizeof(srcMem), &srcMem);
			SetArg(1, sizeof(dstMem), &dstMem);
			SetArg(2, sizeof(height), &height);
			SetArg(3, sizeof(width), &width);
			EnqueueNDRangeKernel(2, global_work_size, local_work_size, 0, NULL, NULL);
#ifdef CL_KERNEL_FINISH
			Finish();
#endif
		}
	}
	else
	{
		CDCT::doTransform(src, dst);
	}
}

CCLIDCT::CCLIDCT(CCLDevice * dev, cl_program program, const char * kernel) :
	CCLDCT(dev, program, kernel)
{}

void CCLIDCT::doTransform(CImage<float> * src, CImage<float> * dst)
{
	CCLDCT::doTransform(src, dst);
}

}
