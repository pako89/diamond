#include <decoder.h>
#include <utils.h>

namespace avlib
{

CDecoder::CDecoder(bool printProgressBar) :
	m_progressBar(printProgressBar)
{
}

CDecoder::~CDecoder()
{
}
	
void CDecoder::printProgressBar(int i, int n)
{
	if(m_progressBar)
	{
		utils::printProgressBar(i, n);
	}
}

}
