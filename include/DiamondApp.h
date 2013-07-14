#ifndef _DIAMOND_APP_H
#define _DIAMOND_APP_H

#include <stdlib.h>
#include <stdio.h>
#include <exception>
#include <string>
#include <stdarg.h>
#include <getopt.h>
#include <utils.h>
#include <image.h>
#include <errno.h>
#include <avlib.h>
#include <encoder.h>

namespace diamond
{

class ExitException : public std::exception
{
public:
	ExitException();
	ExitException(int ret);
	int getValue(void) const throw();
private:
	int m_val;
};

class ParseArgsException : public utils::StringFormatException
{
public:
	ParseArgsException(const char * fmt, ...);
};

enum DiamondOperation
{
	DIAMOND_NOP,
	DIAMOND_OP_ENCODE,
	DIAMOND_OP_DECODE
};

enum EncoderVariant
{
	ENCODER_VARIANT_CPU=0,
	ENCODER_VARIANT_OPENCL=1,
	ENCODER_VARIANT_OPENCL_MERGED=2,
	ENCODER_VARIANT_OPENCL_PARALLEL=3
};

const char * EncoderVariant2Str(enum EncoderVariant v);

struct DiamondConfig
{
	DiamondConfig() :
		Op(DIAMOND_NOP),
		InputFileName("stdin"),
		InputFile(stdin),
		OutputFileName("stdout"),
		OutputFile(stdout),
		ImageType(avlib::IMAGE_TYPE_UNKNOWN),
		ImageTypeStr("unknown"),
		ImageSize(0, 0),
		Variant(ENCODER_VARIANT_OPENCL)
	{}
	DiamondOperation Op;
	const char * InputFileName;
	FILE * InputFile;
	const char * OutputFileName;
	FILE * OutputFile;
	avlib::ImageType ImageType;
	const char * ImageTypeStr;
	avlib::CSize ImageSize;
	avlib::EncoderConfig EncoderConfig;	
	EncoderVariant Variant;
};

/**
 * Singleton class DiamondApp
 */
class CDiamondApp
{
public:
	static CDiamondApp * getInstance(void);

protected:
	static CDiamondApp * m_instance;
	static const struct option common_options[];
	static const struct option encode_options[];
	static const struct option decode_options[];
public:
	~CDiamondApp(void);
	void setName(const char * appName);
	void ParseArgs(int argc, char * argv[]);
	void PrintBanner(void);
	void PrintUsage(void);
	void PrintHelp(void);
	void PrintVersion(void);
	
	const char * getName(void);
	DiamondConfig getConfig(void);
protected:
	CDiamondApp(void);
	DiamondOperation parseOperation(const char * op);
	avlib::HUFFMAN_TYPE parseHuffman(const char * op);
	avlib::ImageType parseImageType(const char * arg);
	bool parseBool(std::string arg);
	std::string getShortOpts(const struct option long_options[], int size);

	DiamondConfig m_config;
	const char * m_appName;
};
}
#endif //_DIAMOND_APP_H

