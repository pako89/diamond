#include <sequence.h>
#include <utils.h>

namespace avlib
{

CSequence::CSequence() :
	m_image(NULL),
	m_fh(NULL)
{
}

CSequence::CSequence(const char * fileName) :
	m_image(NULL),
	m_fh(NULL)
{

}

CSequence::~CSequence()
{
	CloseFile();
	if(NULL != m_image)
	{
		delete m_image;
	}
}

bool CSequence::openFile(const char * file)
{
	m_fh = fopen(file, "rb");
	return (NULL != m_fh);
}

bool CSequence::OpenFile(const char * file)
{
	if(openFile(file))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CSequence::OpenFile(const char * file, ImageType type, int height, int width)
{
	if(openFile(file))
	{
		m_image = new CImage<uint8_t>(type, height, width);
		return read();
	}
	else
	{
		return false;
	}
}

void CSequence::CloseFile()
{
	if(NULL != m_fh)
	{
			fclose(m_fh);
	}
}

CImage<uint8_t> & CSequence::getFrame(void)
{
	return *m_image;
}

bool CSequence::Next(void)
{
	return read();
}

bool CSequence::Seek(int num)
{
	if(num >= 0)
	{		
		return forward(num);
	}
	else
	{
		return back(-num);	
	}
}

bool CSequence::Rewind(int num)
{
	if(num >= 0)
	{		
		return back(num);
	}
	else
	{
		return forward(-num);	
	}
}

bool CSequence::forward(int num)
{
	return true;
}

bool CSequence::back(int num)
{
	return true;
}

bool CSequence::read(void)
{
	if(NULL != m_fh && NULL != m_image)
	{
		if(fread(m_image->operator[](0)[0], m_image->operator[](0).getBytesCount(), 1, m_fh) != 1)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	else
	{
		return false;
	}
}

}

