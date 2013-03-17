#include <sequence.h>
#include <utils.h>
#include <log.h>

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
CSequence::CSequence(FILE * fh) : 
	m_image(NULL),
	m_fh(fh)
{
}

CSequence::CSequence(FILE * fh, ImageType type, int height, int width) :
	m_image(NULL),
	m_fh(fh)
{
	setFormat(type, height, width);
}

CSequence::~CSequence()
{
	CloseFile();
	if(NULL != m_image)
	{
		delete m_image;
	}
}

bool CSequence::setFormat(ImageType type, int height, int width)
{
	if(NULL != m_image)
	{
		delete m_image;
		m_image = NULL;
	}
	if(type != IMAGE_TYPE_UNKNOWN)
	{
		m_image = new CImage<uint8_t>(type, height, width);
		return (NULL != m_image);
	}
	return false;
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
	if(NULL == m_image)
	{
		throw utils::NullReferenceException();
	}
	return *m_image;
}

bool CSequence::WriteNext(void)
{
	return write();
}

bool CSequence::ReadNext(void)
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

bool CSequence::write(void)
{
	if(NULL != m_fh && NULL != m_image)
	{
		int ret = 0;
		for(int k=0;k<m_image->getComponents(); k++)
		{
			ret += fwrite(m_image->operator[](k)[0], m_image->operator[](k).getBytesCount(), 1, m_fh);
		}
		return (ret == m_image->getComponents());
	}
	else
	{
		return false;
	}
}

bool CSequence::read(void)
{
	if(NULL != m_fh && NULL != m_image)
	{
		int ret = 0;
		for(int k=0;k<m_image->getComponents(); k++)
		{
			ret += fread(m_image->operator[](k)[0], m_image->operator[](k).getBytesCount(), 1, m_fh);
		}
		return (ret == m_image->getComponents());
	}
	else
	{
		return false;
	}
}

int CSequence::getHeight(void)
{
	return (*m_image)[0].getHeight();
}

int CSequence::getWidth(void)
{
	return (*m_image)[0].getWidth();
}

size_t CSequence::getFramesCount(void)
{
	size_t frame_size = 0;
	for(int i=0;i<m_image->getComponents(); i++)
	{
		frame_size += (*m_image)[i].getBytesCount();
	}
	long pos = ftell(m_fh);
	dbg("pos=%d\n", pos);
	fseek(m_fh, 0, SEEK_END);
	long count = ftell(m_fh);
	dbg("count=%d\n", count);
	fseek(m_fh, pos, SEEK_SET);
	dbg("frames=%d\n", count/frame_size);
	return (count/frame_size);
}

}

