#ifndef _PROTO_H
#define _PROTO_H

#include <stdint.h>

enum marker_type 
{
	/* Start of stream */
	MARKER_TYPE_SOS		= 0x01,
	MARKER_TYPE_SOF		= 0x02
};

typedef uint16_t marker_type_t;
typedef uint16_t format_t;
typedef uint16_t image_format_t;
typedef uint16_t marker_size_t;
typedef uint8_t macroblock_type_t;

#define MARKER_DEF(name)	typedef struct name##_marker 		\
				{					\
					marker_type_t type;		\
					marker_size_t size;
#define MARKER_END(name)	} name##_marker_t;


MARKER_DEF(sos)
	format_t format;
#define FORMAT_T_IMAGE		0
#define FORMAT_T_DCT		1
#define FORMAT_T_DHUFFMAN	2	
#define FORMAT_T_SHUFFMAN	3
	
	uint8_t huffman;
#define HUFFMAN_T_STATIC	0
#define HUFFMAN_T_DYNAMIC	1

	image_format_t image_format;
#define IMAGE_F_YUV420		0
	
	uint32_t frames_number;
	uint32_t width;
	uint32_t height;
MARKER_END(sos)

MARKER_DEF(sof)
	uint8_t frame_type;
	uint16_t quant_coeff;
MARKER_END(sof)	
/*
MARKER_DEF(som)
	macroblock_type_t type;
	int8_t mx;
	int8_t my;	
MARKED_END(som)
*/
#endif //_PROTO_H
