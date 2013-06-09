#ifndef _COMPONENT_H
#define _COMPONENT_H

#include <stdlib.h>
#include <avlib.h>

namespace avlib
{

template <class T>
class CComponent
{
public:
	CComponent();
	CComponent(CSize size);
	CComponent(int height, int width);
	CComponent(const CComponent<T> & src);
	template <class U> CComponent(const CComponent<U> & src);
	virtual ~CComponent();
	virtual CComponent & operator=(const CComponent & src);
	template <class U> CComponent & operator=(const CComponent<U> & src);
	virtual T * operator[](int height);
	virtual int getWidth(void);
	virtual int getHeight(void);
	virtual int getOriginalWidth(void);
	virtual int getOriginalHeight(void);
	virtual CSize getSize(void);
	virtual CSize getOriginalSize(void);
	virtual int getPointsCount(void);
	virtual bool setSize(CSize size);
	virtual bool setSize(int height, int width);
	virtual size_t getBytesCount(void);
protected:
	virtual void copy(const CComponent<T> * src);
	void release();
	T * m_data;
	CSize m_size;
	CSize m_original_size;
	size_t m_bytes;

template <class> friend class CComponent;
};

}
#endif //_COMPONENT_H
