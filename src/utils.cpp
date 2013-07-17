#include <utils.h>
#include <stdio.h>
#include <cmath>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <string>

#ifndef WIN32
#include <sys/ioctl.h>
#include <unistd.h>
#endif

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
#ifndef WIN32
	if(i==n)
	{
		logev(1, "\n");
	}
	else
	{
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
		logev(1, "\r[%-*s]["YELLOW"%s"EC"]", bars, bar, frames);
		delete line;
		delete frames;
		delete bar;
	}
#else
	logev(1, "\r%d/%d", i, n);
#endif
}
std::string get_src_from_file(char * file_name)
{
	std::fstream fs(file_name,(std::fstream::in | std::fstream::binary));
	std::string s;
	if(fs.is_open()) 
	{
		size_t file_size;
		fs.seekg(0,std::fstream::end);
		file_size = (size_t)fs.tellg();
		fs.seekg(0,std::fstream::beg);
		char * str = new char[file_size+1];
		str[file_size] = '\0';
		if(NULL == str) 
		{
			fs.close();
			return std::string();
		}
		fs.read(str,file_size);
		fs.close();
		s = str;
		delete [] str;
	} 
	else 
	{
		throw utils::StringFormatException("Can not open file: %s\n", file_name);
	}			            
	return s;
}

int ParseInt(std::string arg)
{
	if(arg.size()>0)
	{
		int ret = atoi(arg.c_str());
		if(ret == 0 && arg[0] != '0')
		{
			throw utils::StringFormatException("can not convert '%s' to integer", arg.c_str());
		}
		return ret;
	}
	else
	{
		throw utils::NullReferenceException();
	}
}

std::vector<std::string> & split(const std::string &s, char delim, std::vector<std::string> &elems)
{
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) 
	{
		elems.push_back(item);
	}
	return elems;
}

std::string trim(const std::string& str, const std::string& whitespace)
{
	size_t begin = str.find_first_not_of(whitespace);
	if (begin == std::string::npos) return "";
	size_t end = str.find_last_not_of(whitespace);
	size_t range = end - begin + 1;
	return str.substr(begin, range);
}

std::string bool2str(bool v)
{
	return v?"True":"False";
}

}
