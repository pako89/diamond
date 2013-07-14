#include <sequence.h>
#include <utils.h>
#include <log.h>
#include <string.h>
#include <string>

#define YUV4MPEG_HEADER		"YUV4MPEG2 "
#define YUV4MPEG_HEADER_LEN	10
#define YUV4MPEG_FRAME		"FRAME\n"
#define YUV4MPEG_FRAME_LEN	6

namespace avlib
{

CSequence::CSequence(FILE * fh) : 
	m_image(NULL),
	m_fh(fh),
	m_yuv4mpeg(false)
{
	parseYUV4MPEG();
}

CSequence::CSequence(FILE * fh, ImageType type, int height, int width) :
	m_image(NULL),
	m_fh(fh),
	m_yuv4mpeg(false)
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

void CSequence::setYUV4MPEG(bool val)
{
	m_yuv4mpeg = val;
}

bool CSequence::IsYUV4MPEG()
{
	return m_yuv4mpeg;
}

bool CSequence::parseHeader()
{
	char yuv4mpeg[YUV4MPEG_HEADER_LEN+1] = {0,};
	if(NULL != m_fh)
	{
		if(fread(yuv4mpeg, YUV4MPEG_HEADER_LEN, 1, m_fh) == 1)
		{
			return !strcmp(yuv4mpeg, YUV4MPEG_HEADER);
		}
	}
	return false;
}
	
bool CSequence::writeFrame()
{
	char * frame = (char*)YUV4MPEG_FRAME;
	return fwrite(frame, YUV4MPEG_FRAME_LEN, 1, m_fh) == 1;
}

bool CSequence::readFrame()
{
	char frame[YUV4MPEG_FRAME_LEN+1] = {0,};
	if(NULL != m_fh)
	{
		if(fread(frame, YUV4MPEG_FRAME_LEN, 1, m_fh) == 1)
		{
			return !strcmp(frame, YUV4MPEG_FRAME);
		}
		else
		{
			return false;
		}
	}
}
	
std::string CSequence::readProperties()
{
	std::string ret;
	char c = '\0';
	do
	{
		if(fread(&c, 1, 1, m_fh) == 1)
		{
			if(c!= '\n')
				ret += c;
		}
		else
		{
			throw utils::StringFormatException("YUV4MPEG header corrupted");
		}
	}while(c!='\n');
	return ret;
}

CFrameRate CSequence::getFrameRate()
{
	return m_frameRate;
}
	
void CSequence::setFrameRate(CFrameRate frameRate)
{
	m_frameRate = frameRate;
}

void CSequence::parseYUV4MPEG()
{
	if(parseHeader())
	{
		CImageFormat imageFormat;
		CFrameRate frameRate;
		std::string props = readProperties();
		int pos=0;
		do
		{
			int space = props.find(' ', pos);
			std::string prop = std::string(props, pos, space-pos);
			if(prop.size() > 0)
			{
				char p = prop[0];
				std::string val = std::string(prop, 1, prop.size()-1);
				switch(p)
				{
				case 'W':
					imageFormat.Size.Width = utils::ParseInt(val.c_str());
					break;
				case 'H':
					imageFormat.Size.Height = utils::ParseInt(val.c_str());
					break;
				case 'F':
					frameRate = CFrameRate::ParseFrameRate(val.c_str());
					break;
				case 'C':
					imageFormat.Type = CImageFormat::ParseImageType(val.c_str());
					break;
				default:
					break;
				}
			}
			pos = space+1;
		}while(pos && pos<props.size());
		if(imageFormat.Size.Width > 0
		&& imageFormat.Size.Height > 0
		&& imageFormat.Type != IMAGE_TYPE_UNKNOWN
		&& frameRate.Nom > 0
		&& frameRate.Denom > 0)
		{
			m_yuv4mpeg=true;
			m_frameRate = frameRate;
			setFormat(imageFormat.Type, imageFormat.Size.Height, imageFormat.Size.Width);
		}
	}
	else
	{
		fseek(m_fh, 0, SEEK_SET);
	}
}
	
void CSequence::WriteYUV4MPEG()
{
	CImageFormat imageFormat = getFormat();
#define BUFF_SIZE	2048
	char buff[BUFF_SIZE] = {0,};
	int s = snprintf(buff, BUFF_SIZE, "%sW%d H%d C%s F%d:%d\n",
			YUV4MPEG_HEADER,
			imageFormat.Size.Width,
			imageFormat.Size.Height,
			"420jpeg",
			m_frameRate.Nom,
			m_frameRate.Denom);
	if(fwrite(buff, s, 1, m_fh) != 1)
	{
		throw utils::StringFormatException("Can not write YUV4MPEG Header: '%s'", buff);
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
	else
	{
		throw utils::StringFormatException("invalid format for sequence [height, width or type options]");
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

CImage<uint8_t> * CSequence::getFrame(void)
{
	if(NULL == m_image)
	{
		throw utils::NullReferenceException();
	}
	return m_image;
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
		if(m_yuv4mpeg && !writeFrame())
		{
			throw utils::StringFormatException("Can not write YUV4MPEG FRAME");
		}
		int ret = 0;
		for(int k=0;k<m_image->getComponents(); k++)
		{
			if((*m_image)[k].getOriginalSize() != (*m_image)[k].getSize())
			{
				for(int y = 0 ; y < (*m_image)[k].getOriginalHeight(); y++)
				{
					ret += fwrite((*m_image)[k][y], (*m_image)[k].getOriginalWidth(), 1, m_fh);	
				}
			}
			else
			{
				ret += fwrite((*m_image)[k][0], m_image->operator[](k).getBytesCount(), 1, m_fh);
			}
		}
		return true; //TODO:(ret == m_image->getComponents());
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
		if(m_yuv4mpeg && !readFrame())
		{
			throw utils::StringFormatException("Can not find YUV4MPEG FRAME\n");
		}
		int ret = 0;
		for(int k=0;k<m_image->getComponents(); k++)
		{
			if((*m_image)[k].getOriginalSize() != (*m_image)[k].getSize())
			{
				int o_height = (*m_image)[k].getOriginalHeight();
				int o_width = (*m_image)[k].getOriginalWidth();
				int width = (*m_image)[k].getWidth();
				int height = (*m_image)[k].getHeight();
				for(int y=0;y<o_height; y++)
				{
					ret += fread((*m_image)[k][y], o_width, 1, m_fh);
					//memset(&(*m_image)[k][y][o_width], 0, width-o_width);
				}
			}
			else
			{
				ret += fread((*m_image)[k][0], (*m_image)[k].getBytesCount(), 1, m_fh);
			}
		}
		return true; //TODO: (ret == m_image->getComponents());
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

avlib::CImageFormat CSequence::getFormat(void)
{
	if(NULL != m_image)
	{
		return m_image->getFormat();
	}
	else
	{
		return CImageFormat();
	}
}

size_t CSequence::getFramesCount(void)
{
	size_t frame_size = 0;
	for(int i=0;i<m_image->getComponents(); i++)
	{
		frame_size += (*m_image)[i].getBytesCount();
	}
	long pos = ftell(m_fh);
	fseek(m_fh, 0, SEEK_END);
	long count = ftell(m_fh);
	fseek(m_fh, pos, SEEK_SET);
	return (count/frame_size);
}

}

