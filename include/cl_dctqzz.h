#ifndef _CL_DCTQZZ_H
#define _CL_DCTQZZ_H

#include <transform.h>
#include <utils.h>
#include <avlib.h>
#include <cl_kernel.h>
#include <quantizer.h>
#include <zigzag.h>
#include <cl_image.h>
#include <cl_quantizer.h>

namespace avlib
{

class CCLDCTQZZ : public CTransform<float, int16_t>
{
public:
	CCLDCTQZZ(CCLDevice * dev, cl_program program, const char * kernel);
	virtual ~CCLDCTQZZ();
	virtual void setTables(int qp);
	virtual void setTables(const uint8_t * YQ, const uint8_t * UQ, const uint8_t * VQ, int qp);
protected:
	virtual void doTransform(CImage<float> * src, CImage<int16_t> * dst);
	CCLKernel * m_kernel;
	CCLImage<float> * m_q;
};

class CCLIDCTQ : public CTransform<float, float>
{
public:
	CCLIDCTQ(CCLDevice * dev, cl_program program, const char * kernel);
	virtual ~CCLIDCTQ();
	virtual void setTables(int qp);
	virtual void setTables(const uint8_t * YQ, const uint8_t * UQ, const uint8_t * VQ, int qp);
protected:
	virtual void doTransform(CImage<float> * src, CImage<float> * dst);
	CCLKernel * m_kernel;
	CCLImage<float> * m_q;
};

}


#endif //_CL_DCTQZZ_H
