#include <cl_dctqzz.h>
#include <quantizer.h>

namespace avlib
{

CCLDCTQZZ::CCLDCTQZZ(CCLDevice * dev, cl_program program, const char * kernel)
{
	m_kernel = new ICLKernel(dev, program, kernel);
	m_q = new CCLImage<float>(dev, CSize(8, 8), CL_MEM_READ_ONLY);
}

CCLDCTQZZ::~CCLDCTQZZ()
{
	RELEASE(m_kernel);
	RELEASE(m_q);
}

void CCLDCTQZZ::setTables(int qp)
{
	setTables(default_YQTable, default_UQTable, default_VQTable, qp);
}

void CCLDCTQZZ::setTables(const uint8_t * YQ, const uint8_t * UQ, const uint8_t * VQ, int qp)
{
	for(int i=0;i<64;i++)
	{
		(*m_q)[0][0][i] = 1.0f/((float)YQ[i]*qp);
		(*m_q)[1][0][i] = 1.0f/((float)UQ[i]*qp);
		(*m_q)[2][0][i] = 1.0f/((float)VQ[i]*qp);
	}
}

void CCLDCTQZZ::doTransform(CImage<float> * src, CImage<int16_t> * dst)
{
	CCLImage<float> * clSrc = dynamic_cast<CCLImage<float>*>(src);
	CCLImage<int16_t> * clDst = dynamic_cast<CCLImage<int16_t>*>(dst);
	if(m_kernel && clSrc && clDst)
	{
		for(int i=0;i<clSrc->getComponents(); i++)
		{
			int height = (*clSrc)[i].getHeight();
			int width = (*clSrc)[i].getWidth();
			cl_mem srcMem = clSrc->getCLComponent(i).getCLMem(true);
			cl_mem dstMem = clDst->getCLComponent(i).getCLMem(false);
			cl_mem qMem = m_q->getCLComponent(i).getCLMem(true);
			size_t global_work_size[2];
			global_work_size[0] = height;
			global_work_size[1] = width/8;
			size_t local_work_size[2];
			local_work_size[0] = 8;
			local_work_size[1] = 1;

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
		throw utils::StringFormatException("Can not perform DCT, Quant and ZZ\n");
	}
}

CCLIDCTQ::CCLIDCTQ(CCLDevice * dev, cl_program program, const char * kernel)
{
	m_kernel = new ICLKernel(dev, program, kernel);
	m_q = new CCLImage<float>(dev, CSize(8, 8), CL_MEM_READ_ONLY);
}

CCLIDCTQ::~CCLIDCTQ()
{
	RELEASE(m_kernel);
	RELEASE(m_q);
}

void CCLIDCTQ::setTables(int qp)
{
	setTables(default_YQTable, default_UQTable, default_VQTable, qp);
}

void CCLIDCTQ::setTables(const uint8_t * YQ, const uint8_t * UQ, const uint8_t * VQ, int qp)
{
	for(int i=0;i<64;i++)
	{
		(*m_q)[0][0][i] = ((float)YQ[i]*qp);
		(*m_q)[1][0][i] = ((float)UQ[i]*qp);
		(*m_q)[2][0][i] = ((float)VQ[i]*qp);
	}
}

void CCLIDCTQ::doTransform(CImage<float> * src, CImage<float> * dst)
{
	CCLImage<float> * clSrc = dynamic_cast<CCLImage<float>*>(src);
	CCLImage<float> * clDst = dynamic_cast<CCLImage<float>*>(dst);
	if(m_kernel && clSrc && clDst)
	{
		for(int i=0;i<clSrc->getComponents(); i++)
		{
			int height = (*clSrc)[i].getHeight();
			int width = (*clSrc)[i].getWidth();
			cl_mem srcMem = clSrc->getCLComponent(i).getCLMem(true);
			cl_mem dstMem = clDst->getCLComponent(i).getCLMem(true);
			cl_mem qMem = m_q->getCLComponent(i).getCLMem(true);
			size_t global_work_size[2];
			global_work_size[0] = height;
			global_work_size[1] = width/8;
			size_t local_work_size[2];
			local_work_size[0] = 8;
			local_work_size[1] = 1;

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
		throw utils::StringFormatException("Can not perform DCT, Quant and ZZ\n");
	}
}

}
