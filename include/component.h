#ifndef _COMPONENT_H
#define _COMPONENT_H

#include <stdlib.h>

namespace avlib
{

template <class T>
class CComponent
{
public:
	CComponent();
	CComponent(int height, int width);
	virtual ~CComponent();
	virtual T * operator[](int height);
	virtual int getWidth(void);
	virtual int getHeight(void);
	virtual bool setSize(int height, int width);
	virtual size_t getBytesCount(void);
protected:
	T * m_data;
	int m_width;
	int m_height;
	size_t m_bytes;
	void release();
};

}
#endif //_COMPONENT_H
