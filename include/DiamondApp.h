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
	
	const char * getName(void);
	const char * getInputFileName(void);
	const char * getOutputFileName(void);
	FILE * getInputFile(void);
	FILE * getOutputFile(void);
	avlib::ImageType getImageType(void);
	const char * getImageTypeStr(void);
	int getHeight(void);
	int getWidth(void);
	bool UseOpenCL(void);
	DiamondOperation getOperation(void);
protected:
	CDiamondApp(void);
	DiamondOperation parseOperation(const char * op);
	int parseInt(const char * arg);
	avlib::ImageType parseImageType(const char * arg);
	std::string getShortOpts(const struct option long_options[], int size);

	DiamondOperation m_op;
	const char * m_appName;
	const char * m_inputFileName;
	FILE * m_inputFile;
	const char * m_outputFileName;
	FILE * m_outputFile;
	avlib::ImageType m_imageType;
	const char * m_imageTypeStr;
	int m_imageHeight;
	int m_imageWidth;
	bool m_opencl;
};

}
#endif //_DIAMOND_APP_H

