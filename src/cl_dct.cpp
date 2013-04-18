#include <cl_dct.h>


namespace avlib
{

CCLDCT::CCLDCT(cl_handle h, cl_program program, const char * kernel) :
	m_h(h),
	m_program(program)
{
	cl_int err;
	m_kernel = clCreateKernel(m_program, kernel, &err);
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
	CCLImage<float> * temp = new CCLImage<float>(this->m_h, src->getFormat());
	cl_int err = 0;
	if(m_kernel)
	{
		clSrc->CopyToDevice();
		for(int i=0;i<clSrc->getComponents(); i++)
		{
			int height = (*clSrc)[i].getHeight();
			int width = (*clSrc)[i].getWidth();
			cl_mem srcMem = clSrc->getCLComponent(i).getCLMem();
			cl_mem tmpMem = temp->getCLComponent(i).getCLMem();
			cl_mem dstMem = clDst->getCLComponent(i).getCLMem();
			int srcStep = 1;
			int dstStep = 1;
			size_t global_work_size[2];
			global_work_size[0] = height;
			global_work_size[1] = width/8;
			size_t local_work_size[2];
			local_work_size[0] = 1;
			local_work_size[1] = 1;
			
			err = clSetKernelArg(m_kernel, 0, sizeof(srcMem), &srcMem);
			if(CL_SUCCESS != err) throw utils::StringFormatException("clSetKernelArg(%d)\n", err);
			
			err = clSetKernelArg(m_kernel, 1, sizeof(srcStep), &srcStep);
			if(CL_SUCCESS != err) throw utils::StringFormatException("clSetKernelArg(%d)\n", err);
			
			err = clSetKernelArg(m_kernel, 2, sizeof(tmpMem), &tmpMem);
			if(CL_SUCCESS != err) throw utils::StringFormatException("clSetKernelArg(%d)\n", err);
			
			err = clSetKernelArg(m_kernel, 3, sizeof(dstStep), &dstStep);
			if(CL_SUCCESS != err) throw utils::StringFormatException("clSetKernelArg(%d)\n", err);
			
			err = clSetKernelArg(m_kernel, 4, sizeof(width), &width);
			if(CL_SUCCESS != err) throw utils::StringFormatException("clSetKernelArg(%d)\n", err);
			
			err = clEnqueueNDRangeKernel(m_h.queue, m_kernel, 2, NULL, global_work_size, local_work_size, 0, NULL, NULL);
			if(CL_SUCCESS != err) throw utils::StringFormatException("clEnqueueNDRangeKernel(%d)\n", err);
			
			err = clFinish(m_h.queue);
			if(CL_SUCCESS != err) throw utils::StringFormatException("clFinish1(%d)\n", err);

			srcStep = width;
			dstStep = width;
			global_work_size[0] = height/8;
			global_work_size[1] = width;

			err = clSetKernelArg(m_kernel, 0, sizeof(tmpMem), &tmpMem);
			if(CL_SUCCESS != err) throw utils::StringFormatException("clSetKernelArg(%d)\n", err);
			
			err = clSetKernelArg(m_kernel, 1, sizeof(srcStep), &srcStep);
			if(CL_SUCCESS != err) throw utils::StringFormatException("clSetKernelArg(%d)\n", err);
			
			err = clSetKernelArg(m_kernel, 2, sizeof(dstMem), &dstMem);
			if(CL_SUCCESS != err) throw utils::StringFormatException("clSetKernelArg(%d)\n", err);
			
			err = clSetKernelArg(m_kernel, 3, sizeof(dstStep), &dstStep);
			if(CL_SUCCESS != err) throw utils::StringFormatException("clSetKernelArg(%d)\n", err);
			
			err = clSetKernelArg(m_kernel, 4, sizeof(width), &width);
			if(CL_SUCCESS != err) throw utils::StringFormatException("clSetKernelArg(%d)\n", err);
			
			err = clEnqueueNDRangeKernel(m_h.queue, m_kernel, 2, NULL, global_work_size, local_work_size, 0, NULL, NULL);
			if(CL_SUCCESS != err) throw utils::StringFormatException("clEnqueueNDRangeKernel(%d)\n", err);
			
			err = clFinish(m_h.queue);
			if(CL_SUCCESS != err) throw utils::StringFormatException("clFinish2(%d)\n", err);
		}
		clDst->CopyToHost();
	}
	else
	{
		CDCT::Transform(src, dst);
	}
	delete temp;

}

}
