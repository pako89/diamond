#ifndef _IRENA_APP_H
#define _IRENA_APP_H

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
#include <psnr.h>
#include <list>

namespace irena
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

enum Operation
{
	NOP,
	OP_ENCODE,
	OP_DECODE,
	OP_PSNR,
	OP_INFO
};

enum EncoderVariant
{
	ENCODER_VARIANT_CPU=0,
	ENCODER_VARIANT_OPENCL=1,
	ENCODER_VARIANT_OPENCL_MERGED=2,
	ENCODER_VARIANT_OPENCL_PARALLEL=3
};

const char * EncoderVariant2Str(enum EncoderVariant v);

struct Config
{
	Config() :
		Op(NOP),
		InputFileName("stdin"),
		InputFile(stdin),
		OutputFileName("stdout"),
		OutputFile(stdout),
		ImageType(avlib::IMAGE_TYPE_UNKNOWN),
		ImageTypeStr("unknown"),
		ImageSize(0, 0),
		Variant(ENCODER_VARIANT_OPENCL),
		FrameRate(24, 1)
	{}
	Operation Op;
	const char * InputFileName;
	FILE * InputFile;
	const char * OutputFileName;
	FILE * OutputFile;
	avlib::ImageType ImageType;
	const char * ImageTypeStr;
	avlib::CSize ImageSize;
	avlib::CFrameRate FrameRate;
	avlib::EncoderConfig EncoderConfig;	
	EncoderVariant Variant;
	utils::PSNRConfig PSNRConfig;
};

/**
 * Singleton class CApplication
 */
class CApplication
{
public:
	static CApplication * getInstance(void);
	static void releaseInstance(void);
	static void PrintOpenCLInfo(void);
	static void PrintCPUInfo(void);
	static void PrintMemInfo(void);
	typedef std::list<std::pair<std::string, std::string> > props_t;
	static CApplication::props_t GetProcInfo(const char * procname);
protected:
	static CApplication * m_instance;
	static const struct option common_options[];
	static const struct option encoder_options[];
	static const struct option decoder_options[];
	static const struct option psnr_options[];
	void appendLongOptions(std::list<option> & options, const option * long_options, int c);
public:
	~CApplication(void);
	void setName(const char * appName);
	void ParseArgs(int argc, char * argv[]);
	void PrintBanner(void);
	void PrintUsage(void);
	void PrintHelp(void);
	void PrintVersion(void);
	const char * getName(void);
	Config getConfig(void);
protected:
	CApplication(void);
	Operation parseOperation(std::string op);
	cl_device_type parseDevice(std::string dev);
	avlib::HUFFMAN_TYPE parseHuffman(const char * op);
	avlib::ImageType parseImageType(const char * arg);
	bool parseBool(std::string arg);
	std::string getShortOpts(const struct option long_options[], int size);

	Config m_config;
	const char * m_appName;
};
}
#endif //_IRENA_APP_H

