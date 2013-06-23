#include <bitstream.h>
#include <log.h>

namespace avlib
{

CBitstream::CBitstream(int32_t size)
	:mSize(size),mPosition(0),mBits(0)
{
	this->fh = NULL;
	if(this->mSize<0)
		this->mSize = -this->mSize;
	this->mBuffer = new uint8_t[this->mSize];
	this->mBufferPtr = this->mBuffer;
	memset(this->mBuffer,0,this->mSize);
}

CBitstream::~CBitstream()
{
	this->fh=NULL;
	delete [] this->mBuffer;
}

int32_t CBitstream::getSize()
{
	return this->mSize;
}

int32_t CBitstream::getPosition()
{
	return this->mPosition;
}

bool CBitstream::setPosition(int32_t pos)
{
	if(pos < 0 || pos > (this->mSize<<3))
	{
		return false;
	}
	this->mPosition = pos;
	this->mBits = pos%8;
	this->mBufferPtr = &this->mBuffer[pos/8];
	return true;
}

bool CBitstream::putBits(int32_t n,uint32_t bits)
{
	if(n>32)
	{
		fprintf(stderr,"CBitstream: getBits: n > 32\n");
		exit(1);
	}
	if(n>32 || n < 0 )
		return false;
	uint32_t	_bits_temp;
	int32_t	_bits_cnt;
	uint64_t	_bits_buff;
	
	_bits_cnt = this->mBits;
	_bits_buff = (uint64_t)(*this->mBufferPtr)<<(24+_bits_cnt);
	_bits_temp = (bits<<(32-n));
	_bits_buff = (_bits_buff | _bits_temp)<<(32-_bits_cnt); 
	_bits_cnt += n;
	
	this->mBits = _bits_cnt&0x7;
	this->mPosition += n;
	
	*(this->mBufferPtr++) = (uint8_t)(_bits_buff>>56);
	if(_bits_cnt>8)	*(this->mBufferPtr++) = (uint8_t)(_bits_buff>>48);
	if(_bits_cnt>16)	*(this->mBufferPtr++) = (uint8_t)(_bits_buff>>40);
	if(_bits_cnt>24)	*(this->mBufferPtr++) = (uint8_t)(_bits_buff>>32);
	if(_bits_cnt>32)	*(this->mBufferPtr++) = (uint8_t)(_bits_buff>>24);
	if(_bits_cnt>48)	*(this->mBufferPtr++) = (uint8_t)(_bits_buff>>16);
	
	if(_bits_cnt&7)	this->mBufferPtr--;
	
	return this->flush();
}

uint32_t CBitstream::getBitsQ(int32_t n,uint32_t * bits)
{
	if(n>32)
	{
		fprintf(stderr,"CBitstream: getBits: n > 32\n");
		exit(1);
	}
	uint64_t _bits_buff = 0;
	uint32_t _bits_temp;
	int32_t  _bits_cnt;
	
	_bits_cnt = this->mPosition%8;
	_bits_temp = _bits_cnt + n;
	
	_bits_buff = ((uint64_t)(*(this->mBufferPtr++))) << 32;
	if(_bits_temp>8) _bits_buff = (_bits_buff) | ((uint64_t)(*(this->mBufferPtr++))<<24);
	if(_bits_temp>16) _bits_buff = (_bits_buff) | ((uint64_t)(*(this->mBufferPtr++))<<16);
	if(_bits_temp>24) _bits_buff = (_bits_buff) | ((uint64_t)(*(this->mBufferPtr++))<<8);
	if(_bits_temp>32) _bits_buff = (_bits_buff) | ((uint64_t)(*(this->mBufferPtr++)));
	
	_bits_buff =  	(uint64_t)(_bits_buff)<<(24+_bits_cnt);
	
	_bits_cnt = _bits_cnt + n;
	this->mBits = _bits_cnt&0x7;
	this->mPosition += n;
	if(_bits_cnt&7) this->mBufferPtr--;
	*bits =  (uint32_t)(_bits_buff >> (32+32-n));
	return this->fill();
}

uint32_t CBitstream::getBits(int32_t n)
{
	if(n>32)
	{
		fprintf(stderr,"CBitstream: getBits: n > 32\n");
		exit(1);
	}
	uint64_t _bits_buff = 0;
	uint32_t _bits_temp;
	int32_t  _bits_cnt;
	
	_bits_cnt = this->mPosition%8;
	_bits_temp = _bits_cnt + n;
	
	_bits_buff = ((uint64_t)(*(this->mBufferPtr++))) << 32;	
	if(_bits_temp>8) _bits_buff = (_bits_buff) | ((uint64_t)(*(this->mBufferPtr++))<<24);
	if(_bits_temp>16) _bits_buff = (_bits_buff) | ((uint64_t)(*(this->mBufferPtr++))<<16);
	if(_bits_temp>24) _bits_buff = (_bits_buff) | ((uint64_t)(*(this->mBufferPtr++))<<8);
	if(_bits_temp>32) _bits_buff = (_bits_buff) | ((uint64_t)(*(this->mBufferPtr++)));
	
	_bits_buff =  (uint64_t)(_bits_buff)<<(24+_bits_cnt);
	_bits_cnt = _bits_cnt + n;
	this->mBits = _bits_cnt&0x7;
	this->mPosition += n;
	if(_bits_cnt&7) this->mBufferPtr--;
	
	this->fill();
	return (uint32_t)(_bits_buff >> (32+32-n));
}

void CBitstream::set_fh_fill(FILE * fh)
{
	if(fh!=NULL)
	{

		this->fh=fh;
		/*if((ret = fread(this->mBuffer,1,this->mSize,this->fh)) != this->mSize)
		{
			perror("set_fh:fill");
			exit(1);
		}
		*/
		fread(this->mBuffer, 1, this->mSize, this->fh);
	}

}

void CBitstream::set_fh(FILE * fh)
{
	if(fh!=NULL)
	{
		this->fh=fh;
	}	
}


bool CBitstream::write_block(void * ptr,uint32_t size)
{
	uint8_t * bptr = (uint8_t*)ptr;
	while(size--)
	{
		this->putBits(8,*(bptr++));
	}
	return true;    
}

bool CBitstream::read_block(void * ptr,uint32_t size)
{
	uint8_t * bptr = (uint8_t*)ptr;
	while(size--)
	{
		*(bptr++) = (uint8_t)this->getBits(8);
	}
	return true;
}

bool CBitstream::flush_all()
{ 
	if(this->fh!=NULL)
	{
		size_t pos = this->mPosition/8;
		if(pos != fwrite(this->mBuffer,1,pos,this->fh))
		{
			perror("CBitstream->flush_all: fwrite()");
			exit(1);
		}
		if(this->mBits)
		{
			if(fwrite(this->mBufferPtr,sizeof(uint8_t),1,this->fh) != 1)
			{
				perror("CBitstream->flush_all: fwrite()");
				exit(1);
			}
		}
		this->mPosition=0;
		this->mBits=0;
		return true;
	}
	return false;
}

bool CBitstream::flush()
{
	size_t pos;
	if((pos=(this->mPosition/8)) < (size_t)(this->mSize-4))
		return true;
	if(this->fh != NULL)
	{
		if(pos!=fwrite(this->mBuffer,1,pos,this->fh))
		{
			fprintf(stderr,"CBitstream->flush(): fwrite:");
			exit(1);
		}
		if(this->mBufferPtr != &this->mBuffer[pos])
		{
			fprintf(stderr,"UWAGA this->mBufferPtr != &this->mBuffer[pos]\n");
			exit(1);
		}
		*(this->mBuffer) = *(this->mBufferPtr);
		this->mBufferPtr = this->mBuffer;
		this->mPosition = this->mBits;
		memset(&this->mBuffer[1],0,this->mSize-1);
		return true;
	}
	return false;
}

bool CBitstream::fill()
{
	int pos;
	if((pos=this->mPosition/8) <= (this->mSize-4))
		return true;
	if(this->fh != NULL)
	{
		int left = this->mSize-pos;
		memcpy(this->mBuffer,&this->mBuffer[pos],left);
		int ret;
		if((ret=fread(&this->mBuffer[left],sizeof(uint8_t),pos,this->fh))<0)
		{
			perror("CBitstream->fill: fread()");
			exit(1);
		}
		this->mBufferPtr = this->mBuffer;
		this->mPosition = this->mBits;
    	
		return ret!=0;      
  }
  return false;
}

}
