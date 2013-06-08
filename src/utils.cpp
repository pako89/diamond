#include <utils.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <cmath>
#include <stdlib.h>
#include <string.h>

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
	m_msg = utils::string_format(fmt, argptr);
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

int log2(int val)
{
	int result = 0;
	if(val == 1)
	{
		return 0;
	}
	else
	{
		while(val>>++result > 1);
		return result;
	}
}

void printProgressBar(int i, int n)
{
#if 1
	struct winsize w;
	i++;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	int cols = w.ws_col;
	char * line = new char[cols+1];
	int nsize = log10(n)+1;
	int fsize = 2*nsize+2;
	char * frames = new char[fsize];
	snprintf(frames, fsize, "%*d/%*d", nsize, i, nsize, n);
	int bars = cols-fsize-3;
	int ni = i*bars/n;
	if(i!=n) ni++;
	char * bar = new char[ni+1];
	memset(bar, '=', ni);
	int mi = i%4;
	if(ni>0)
	{
		bar[ni-1] = (i==n)?'=':(mi==0)?'|':(mi==1)?'/':(mi==2)?'-':'\\';
	}
	bar[ni]=0;
	LOG("\r[%-*s]["YELLOW"%s"EC"]", bars, bar, frames);
	delete line;
	delete frames;
	delete bar;
#else
	LOG("\r%d/%d", i+1, n);
#endif
}

}
