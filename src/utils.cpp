#include <utils.h>

namespace diamond
{
namespace utils
{

NullReferenceException::NullReferenceException()
{
}

const char * NullReferenceException::what() const throw()
{
	return "Null reference exception";
}

StringFormatException::StringFormatException() : 
	m_msg("Some exception occured...\n")
{
}

StringFormatException::StringFormatException(const char * fmt, ...)
{
	va_list argptr;
	va_start(argptr, fmt);
	m_msg = diamond::utils::string_format(fmt, argptr);
	va_end(argptr);
}

StringFormatException::~StringFormatException() throw()
{
}

const char * StringFormatException::what() const throw()
{
	return m_msg.c_str();
}

std::string string_format(const char* fmt, va_list vl)
{
	int size = 512;
	char* buffer = 0;
	buffer = new char[size];
	int nsize = vsnprintf(buffer, size, fmt, vl);
	if(size<=nsize)
	{
		delete[] buffer;
		buffer = 0;
		buffer = new char[nsize+1];
		nsize = vsnprintf(buffer, size, fmt, vl);
	}
	std::string ret(buffer);
	delete[] buffer;
	return ret;
}

std::string string_format(const char* fmt, ...)
{
	va_list vl;
	va_start(vl, fmt);
	std::string ret = string_format(fmt, vl);
	va_end(vl);
	return ret;
}

}
}
