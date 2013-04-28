#ifndef _UTILS_H
#define _UTILS_H

#include <string>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <exception>
#include <limits>
#include <image.h>
#include <log.h>

#define INSTANTIATE(t_class, t_arg)		template class t_class<t_arg>
#define INSTANTIATE2(t_class, t_arg1, t_arg2)	template class t_class<t_arg1, t_arg2>
#ifdef WIN32
#define CONVERSION(t_class, t_from, t_to) 	template t_class<t_to>::t_class(const t_class<t_from> &);	\
						template t_class<t_to> & t_class<t_to>::operator=(const t_class<t_from> & src)
#else
#define CONVERSION(t_class, t_from, t_to) 	template t_class<t_to>::t_class<t_from>(const t_class<t_from> &);	\
						template t_class<t_to> & t_class<t_to>::operator=(const t_class<t_from> & src)
#endif
#define ARRAY_SIZE(x)				(sizeof((x))/sizeof((x)[0]))

namespace utils
{

class NullReferenceException : public std::exception
{
public:
	NullReferenceException();
	virtual const char * what() const throw();
};

class StringFormatException : public std::exception
{
public:
	StringFormatException();
	StringFormatException(const char * fmt, ...);
	virtual ~StringFormatException() throw();
	virtual const char * what() const throw();
protected:
	std::string m_msg;
};

std::string string_format(const char* fmt, va_list vl);
std::string string_format(const char * fmt, ...);

template <class T> void swap(T x1, T x2)
{
	T temp = x1;
	x1 = x2;
	x2 = temp;
}

template <class T, class U> 
T clamp(U val)
{
	if(val > std::numeric_limits<T>::max()) {
		return std::numeric_limits<T>::max();
	}
	if(val < std::numeric_limits<T>::min()) {
		return std::numeric_limits<T>::min();
	}
	return (T)val;
}

template <class T>
T max(T v1, T v2)
{
	return (v1 > v2) ? v1 : v2;
}

template <class T>
T min(T v1, T v2)
{
	return (v1 < v2) ? v1 : v2;
}

template <class T>
T product(T * ptr, int num)
{
	if(num > 0)
	{
		T ret = ptr[0];
		for(int i=1; i < num; i++)
		{
			ret *= ptr[i];
		}
		return ret;
	}
}

template <class T, class U> 
void clampImg(avlib::CImage<T> * src, avlib::CImage<U> * dst)
{
	if(src->getFormat() != dst->getFormat())
	{
		throw utils::StringFormatException("formats does not match\n");
	}
	for(int k=0;k<src->getComponents(); k++)
	{
		for(int y=0; y < (*src)[k].getHeight(); y++)
		{
			for(int x=0; x < (*src)[k].getWidth(); x++)
			{
				(*dst)[k][y][x] = clamp<U,T>((*src)[k][y][x]);
			}
		}
		
	}
}

}


#endif //_UTILS_H
