#ifndef _SEQUENCE_H
#define _SEQUENCE_H

#include <image.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <string>

namespace avlib
{

class CSequence
{
public:
	CSequence(FILE * fh);
	CSequence(FILE * fh, ImageType type, int height, int width);
	virtual ~CSequence();
	bool setFormat(ImageType type, int height, int width);
	CImage<uint8_t> * getFrame(void);
	bool ReadNext(void);
	bool WriteNext(void);
	bool Seek(int num);
	bool Rewind(int num);
	int getHeight(void);
	int getWidth(void);
	CFrameRate getFrameRate();
	void setFrameRate(CFrameRate frameRate);
	bool IsYUV4MPEG(void);
	void setYUV4MPEG(bool val);
	void WriteYUV4MPEG();
	size_t getFramesCount(void);
	avlib::CImageFormat getFormat(void);
protected:
	std::string readProperties();
	bool parseHeader();
	void parseYUV4MPEG();
	bool readFrame();
	bool writeFrame();
	bool openFile(const char * file);
	bool forward(int num);
	bool back(int num);
	bool read(void);
	bool write(void);

	CImage<uint8_t> * m_image;
	FILE * m_fh;
	size_t m_frameSize;
	bool m_yuv4mpeg;
	CFrameRate m_frameRate;
	int m_yuv4mpegFrameSize;
};

}
#endif //_SEQUENCE_H
