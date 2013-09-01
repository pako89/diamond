#include <cl_quantizer.h>

namespace avlib
{

CCLQuant::CCLQuant(CCLDevice * dev, cl_program program, const char * kernel) : 
	m_kernel(NULL)
{
	m_kernel = new CCLKernel(dev, program, kernel);
}

void CCLQuant::setTables(int qp)
{
	CQuant::setTables(qp);
}

void CCLQuant::setTables(const uint8_t * YQ, const uint8_t * UQ, const uint8_t * VQ, int qp)
{
	if(NULL == this->m_q) this->m_q = new CCLImage<float>(this->m_kernel->getDevice(), CSize(8, 8), CL_MEM_READ_ONLY);
	CQuant::setTables(YQ, UQ, VQ, qp);
	dynamic_cast<CCLImage<float>*>(m_q)->CopyToDevice();
}

void CCLQuant::doTransform(CImage<float> * src, CImage<float> * dst)
{
	CCLImage<float> * clSrc = dynamic_cast<CCLImage<float>*>(src);
	CCLImage<float> * clDst = dynamic_cast<CCLImage<float>*>(dst);
	CCLImage<float> * clQ = dynamic_cast<CCLImage<float>*>(m_q);
	if(this->m_kernel && NULL != clSrc && NULL != clDst && NULL != clQ)
	{
		for(int i=0;i<clSrc->getComponents(); i++)
		{
			int height = (*clSrc)[i].getHeight();
			int width = (*clSrc)[i].getWidth();
			cl_mem srcMem = clSrc->getCLComponent(i).getCLMem(true);
			cl_mem dstMem = clDst->getCLComponent(i).getCLMem(false);
			cl_mem qMem = clQ->getCLComponent(i).getCLMem(true);
			size_t global_work_size[2];
			global_work_size[0] = height;
			global_work_size[1] = width;
			size_t local_work_size[2];
			local_work_size[0] = 8;
			local_work_size[1] = 8;
			
			m_kernel->SetArg(0, sizeof(srcMem), &srcMem);
			m_kernel->SetArg(1, sizeof(dstMem), &dstMem);
			m_kernel->SetArg(2, sizeof(qMem), &qMem);
			m_kernel->SetArg(3, sizeof(height), &height);
			m_kernel->SetArg(4, sizeof(width), &width);
			m_kernel->EnqueueNDRangeKernel(2, global_work_size, local_work_size, 0, NULL, NULL);
#ifdef CL_KERNEL_FINISH
			m_kernel->Finish();
#endif
		}
	}
	else
	{
		CQuant::doTransform(src, dst);
	}
}

CCLIQuant::CCLIQuant(CCLDevice * dev, cl_program program, const char * kernel) :
	m_kernel(NULL)
{
	m_kernel = new CCLKernel(dev, program, kernel);
}

void CCLIQuant::setTables(int qp)
{
	CIQuant::setTables(qp);
}

void CCLIQuant::setTables(const uint8_t * YQ, const uint8_t * UQ, const uint8_t * VQ, int qp)
{
	if(NULL == this->m_q) this->m_q = new CCLImage<float>(this->m_kernel->getDevice(), CSize(8, 8), CL_MEM_READ_ONLY);
	CIQuant::setTables(YQ, UQ, VQ, qp);
	dynamic_cast<CCLImage<float>*>(m_q)->CopyToDevice();

}

void CCLIQuant::doTransform(CImage<float> * src, CImage<float> * dst)
{
	CCLImage<float> * clSrc = dynamic_cast<CCLImage<float>*>(src);
	CCLImage<float> * clDst = dynamic_cast<CCLImage<float>*>(dst);
	CCLImage<float> * clQ = dynamic_cast<CCLImage<float>*>(m_q);
	if(this->m_kernel && NULL != clSrc && NULL != clDst && NULL != clQ)
	{
		for(int i=0;i<clSrc->getComponents(); i++)
		{
			int height = (*clSrc)[i].getHeight();
			int width = (*clSrc)[i].getWidth();
			cl_mem srcMem = clSrc->getCLComponent(i).getCLMem();
			cl_mem dstMem = clDst->getCLComponent(i).getCLMem();
			cl_mem qMem = clQ->getCLComponent(i).getCLMem();
			size_t global_work_size[2];
			global_work_size[0] = height;
			global_work_size[1] = width;
			size_t local_work_size[2];
			local_work_size[0] = 8;
			local_work_size[1] = 8;

			m_kernel->SetArg(0, sizeof(srcMem), &srcMem);
			m_kernel->SetArg(1, sizeof(dstMem), &dstMem);
			m_kernel->SetArg(2, sizeof(qMem), &qMem);
			m_kernel->SetArg(3, sizeof(height), &height);
			m_kernel->SetArg(4, sizeof(width), &width);
			m_kernel->EnqueueNDRangeKernel(2, global_work_size, local_work_size, 0, NULL, NULL);
#ifdef CL_KERNEL_FINISH			
			m_kernel->Finish();
#endif 
		}
	}
	else
	{
		CIQuant::doTransform(src, dst);
	}
}

}
