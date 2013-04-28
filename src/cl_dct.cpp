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

void CCLDCT::Transform(CImage<float> * src, CImage<float> * dst)
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
			cl_mem srcMem = clSrc->getCLComponent(i).getCLMem();
			cl_mem dstMem = clDst->getCLComponent(i).getCLMem();
			size_t global_work_size[2];
			global_work_size[0] = height;
			global_work_size[1] = width/8;
			size_t local_work_size[2];
			local_work_size[0] = 8;
			local_work_size[1] = 1;
			
			err = clSetKernelArg(m_kernel, 0, sizeof(srcMem), &srcMem);
			if(CL_SUCCESS != err) throw utils::StringFormatException("clSetKernelArg(%d)\n", err);
			
			err = clSetKernelArg(m_kernel, 1, sizeof(dstMem), &dstMem);
			if(CL_SUCCESS != err) throw utils::StringFormatException("clSetKernelArg(%d)\n", err);
			
			err = clSetKernelArg(m_kernel, 2, sizeof(height), &height);
			if(CL_SUCCESS != err) throw utils::StringFormatException("clSetKernelArg(%d)\n", err);
			
			err = clSetKernelArg(m_kernel, 3, sizeof(width), &width);
			if(CL_SUCCESS != err) throw utils::StringFormatException("clSetKernelArg(%d)\n", err);
			
			EnqueueNDRangeKernel(2, global_work_size, local_work_size, 0, NULL, NULL);
			
			err = clFinish(m_dev->getCommandQueue());
			if(CL_SUCCESS != err) throw utils::StringFormatException("clFinish(%d)\n", err);
		}
	}
	else
	{
		CDCT::Transform(src, dst);
	}
}

}
