#include <cl_prediction.h>


namespace avlib
{

CCLPrediction::CCLPrediction(CCLDevice * dev) :
	m_dev(dev)
{
}

CCLPrediction::CCLPrediction(CCLDevice * dev, CImageFormat format) :
	m_dev(dev)
{
	m_last = new CCLImage<float>(dev, format);
}


}
