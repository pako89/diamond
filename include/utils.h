#ifndef _UTILS_H
#define _UTILS_H

#include <string>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <exception>

#define INSTANTIATE(t_class, t_arg)		template class t_class<t_arg>

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

}


#endif //_UTILS_H
