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
	CSequence(const char * fileName);
	virtual ~CSequence();
	bool OpenFile(const char * file);
	bool OpenFile(const char * file, ImageType type, int height, int width);
	CImage<uint8_t> & getFrame(void);
	void CloseFile();
	bool Next(void);
	bool Seek(int num);
	bool Rewind(int num);	
protected:
	bool openFile(const char * file);
	bool forward(int num);
	bool back(int num);
	bool read(void);

	CImage<uint8_t> * m_image;
	FILE * m_fh;
	size_t m_frameSize;
};

}
#endif //_SEQUENCE_H