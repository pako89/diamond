#ifndef _INTERPOLATION_H
#define _INTERPOLATION_H

#include <transform.h>
#include <utils.h>
#include <avlib.h>

namespace avlib
{

template <class T>
class CInterpolation : public CTransform<T, T>
{
public:
	CInterpolation();
	CInterpolation(int scale);
	virtual void setScale(int scale);
	virtual int getScale(void);
protected:
	virtual void doTransform(CImage<T> * src, CImage<T> * dst);
	int m_scale;
};

}

#endif //_INTERPOLATION_H
