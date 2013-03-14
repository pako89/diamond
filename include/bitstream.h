#ifndef _BITSTREAM_H
#define _BITSTREAM_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdint.h>

namespace avlib
{

class CBitstream
{
public:
	CBitstream(int32_t size);
	virtual ~CBitstream();	
	int32_t getSize();
	void set_fh(FILE * fh);
	void set_fh_fill(FILE * fh);
	virtual int32_t getPosition();
	virtual bool setPosition(int32_t pos);
	virtual bool putBits(int32_t n,uint32_t bits);
	virtual uint32_t getBitsQ(int32_t n,uint32_t * bits);
	virtual uint32_t getBits(int32_t n);
	virtual bool write_block(void * ptr,uint32_t size);
	virtual bool read_block(void * ptr,uint32_t size); 
	virtual bool flush();
	virtual bool flush_all();
	virtual bool fill();
protected:
	FILE * fh;
	uint8_t* mBuffer;
	uint8_t* mBufferPtr;
	int32_t	 mSize;
	int32_t	 mPosition;
	int32_t	 mBits;
private:
};

}
#endif //_BITSTREAM_H
