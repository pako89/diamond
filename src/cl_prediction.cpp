#include <cl_prediction.h>
#include <cl_interpolation.h>

namespace avlib
{

CCLPrediction::CCLPrediction(CCLDevice * dev) :
	m_dev(dev),
	m_kernelTransform(NULL),
	m_kernelITransform(NULL),
	m_kernelPrediction(NULL)
{
}

CCLPrediction::~CCLPrediction()
{
	RELEASE(m_kernelTransform);
	RELEASE(m_kernelITransform);
	RELEASE(m_kernelPrediction);
}

void CCLPrediction::Init(CImageFormat format, int scale, cl_program program, const char * interpolation_kernel)
{
	if(NULL == m_interpol)
	{
		m_interpol = new CCLInterpolation<float>(m_dev, program, interpolation_kernel);
		m_interpol->setScale(scale);
	}
	if(NULL == m_last)
	{
		CImageFormat newFormat = format;
		newFormat.Size.Width *= scale;
		newFormat.Size.Height *= scale;
		m_last = new CCLImage<float>(m_dev, newFormat);
	}
	CPrediction::Init(format, scale);
}

void CCLPrediction::setTransformKernel(cl_program program, const char * kernel)
{
	m_kernelTransform = new CCLKernel(m_dev, program, kernel);
}

void CCLPrediction::setITransformKernel(cl_program program, const char * kernel)
{
	m_kernelITransform = new CCLKernel(m_dev, program, kernel);
}

void CCLPrediction::setPredictionKernel(cl_program program, const char * kernel)
{
	m_kernelPrediction = new CCLKernel(m_dev, program, kernel);
}

void CCLPrediction::doPredict(CComponent<float> * pSrc, CPredictionInfoTable * pPred)
{
	CCLComponent<float> * clSrc = dynamic_cast<CCLComponent<float>*>(pSrc);
	CCLImage<float> * clLast = dynamic_cast<CCLImage<float>*>(m_last);
	CCLPredictionInfoTable * clInfo = dynamic_cast<CCLPredictionInfoTable*>(pPred);
	if(NULL != m_kernelPrediction &&
	   NULL != clSrc &&
	   NULL != clLast &&
	   NULL != clInfo
	)
	{
		clPredict(clSrc, &clLast->getCLComponent(0), clInfo);
	}
	else
	{
		CPrediction::doPredict(pSrc, pPred);
	}
}

void CCLPrediction::doTransformPFrame(CImage<float> * pSrc, CImage<float> * pDst, CPredictionInfoTable * pPred)
{
	if(NULL != m_kernelTransform)
	{
		clTransform(m_kernelTransform, pSrc, pDst, pPred);
	}
	else
	{
		CPrediction::doTransformPFrame(pSrc, pDst, pPred);
	}
}

void CCLPrediction::doITransformPFrame(CImage<float> * pSrc, CImage<float> * pDst, CPredictionInfoTable * pPred)
{
	if(NULL != m_kernelITransform)
	{
		clTransform(m_kernelITransform, pSrc, pDst, pPred);
	}
	else
	{
		CPrediction::doITransformPFrame(pSrc, pDst, pPred);
	}
}


void CCLPrediction::clTransform(CCLKernel * kernel, CImage<float> * pSrc, CImage<float> * pDst, CPredictionInfoTable * pPred)
{
	CCLImage<float> * clSrc = dynamic_cast<CCLImage<float>*>(pSrc);
	CCLImage<float> * clDst = dynamic_cast<CCLImage<float>*>(pDst);
	CCLImage<float> * clLast = dynamic_cast<CCLImage<float>*>(m_last);
	CCLPredictionInfoTable * clPred = dynamic_cast<CCLPredictionInfoTable*>(pPred);
	if(NULL == clSrc || NULL == clDst || NULL == clPred || NULL == clLast)
	{
		throw utils::StringFormatException("clTransform(): src or dst is not CCLImage<float>\n");
	}
	int predHeight = clPred->getHeight();
	int predWidth = clPred->getWidth();
	for(int k=0;k<clSrc->getComponents();k++)
	{
		int height = (*clSrc)[k].getHeight();
		int width = (*clSrc)[k].getWidth();
		int blockScale = 16/clSrc->getScale(k);
		cl_mem srcMem = clSrc->getCLComponent(k).getCLMem(true);
		cl_mem dstMem = clDst->getCLComponent(k).getCLMem(true);
		cl_mem lastMem = clLast->getCLComponent(k).getCLMem(true);
		cl_mem predMem = clPred->getCLMem(true);
		size_t global_work_size[2];
		global_work_size[0] = height;
		global_work_size[1] = width;

		kernel->SetArg(0, sizeof(srcMem), &srcMem);
		kernel->SetArg(1, sizeof(dstMem), &dstMem);
		kernel->SetArg(2, sizeof(lastMem), &lastMem);
		kernel->SetArg(3, sizeof(predMem), &predMem);
		kernel->SetArg(4, sizeof(width), &width);
		kernel->SetArg(5, sizeof(predWidth), &predWidth);
		kernel->SetArg(6, sizeof(blockScale), &blockScale);
		int lastWidth = (*clLast)[k].getWidth();
		kernel->SetArg(7, sizeof(lastWidth), &lastWidth);
		int interpolScale = m_interpol->getScale();
		kernel->SetArg(8, sizeof(interpolScale), &interpolScale);
		kernel->EnqueueNDRangeKernel(2, global_work_size, NULL, 0, NULL, NULL);
#ifdef CL_KERNEL_FINISH
		kernel->Finish();
#endif
	}
}

void CCLPrediction::clPredict(CCLComponent<float> * pSrc, CCLComponent<float> * pLast, CCLPredictionInfoTable * pPred)
{
	int height = pSrc->getHeight();
	int width = pSrc->getWidth();
	int predHeight = pPred->getHeight();
	int predWidth = pPred->getWidth();
	cl_mem srcMem = pSrc->getCLMem(true);
	cl_mem lastMem = pLast->getCLMem(true);
	cl_mem predMem = pPred->getCLMem(true);
	size_t global_work_size[2] = {predHeight, predWidth};
	
	m_kernelPrediction->SetArg(0, sizeof(srcMem), &srcMem);
	m_kernelPrediction->SetArg(1, sizeof(lastMem), &lastMem);
	m_kernelPrediction->SetArg(2, sizeof(predMem), &predMem);
	m_kernelPrediction->SetArg(3, sizeof(height), &height);
	m_kernelPrediction->SetArg(4, sizeof(width), &width);
	m_kernelPrediction->SetArg(5, sizeof(predHeight), &predHeight);
	m_kernelPrediction->SetArg(6, sizeof(predWidth), &predWidth);
	m_kernelPrediction->SetArg(7, sizeof(m_max), &m_max);
	int lastWidth = pLast->getWidth();
	int lastHeight = pLast->getHeight();
	int scale = getInterpolationScale();
	m_kernelPrediction->SetArg(8, sizeof(lastHeight), &lastHeight);
	m_kernelPrediction->SetArg(9, sizeof(lastWidth), &lastWidth);
	m_kernelPrediction->SetArg(10, sizeof(scale), &scale);
	m_kernelPrediction->EnqueueNDRangeKernel(2, global_work_size, NULL, 0, NULL, NULL);
#ifdef CL_KERNEL_FINISH
	m_kernelPrediction->Finish();
#endif
}
	
CCLImage<float> * CCLPrediction::getLastImage()
{
	return dynamic_cast<CCLImage<float>*>(m_last);
}


}
