#ifndef _SEQUENCE_H
#define _SEQUENCE_H

#include <image.h>
#include <stdio.h>
#include <stdint.h>

namespace avlib
{

class CSequence
{
public:
	CSequence();
	CSequence(FILE * fh);
	CSequence(FILE * fh, ImageType type, int height, int width);
	CSequence(const char * fileName);
	virtual ~CSequence();
	bool setFormat(ImageType type, int height, int width);
	bool OpenFile(const char * file);
	bool OpenFile(const char * file, ImageType type, int height, int width);
	CImage<uint8_t> & getFrame(void);
	void CloseFile();
	bool ReadNext(void);
	bool WriteNext(void);
	bool Seek(int num);
	bool Rewind(int num);
	int getHeight(void);
	int getWidth(void);
	size_t getFramesCount(void);
protected:
	bool openFile(const char * file);
	bool forward(int num);
	bool back(int num);
	bool read(void);
	bool write(void);

	CImage<uint8_t> * m_image;
	FILE * m_fh;
	size_t m_frameSize;
};

}
#endif //_SEQUENCE_H
