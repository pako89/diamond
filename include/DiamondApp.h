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
	static const struct option long_options[];
public:
	~CDiamondApp(void);
	void setName(const char * appName);
	void ParseArgs(int argc, char * argv[]);
	void PrintBanner(void);
	void PrintUsage(void);
	void PrintHelp(void);

	const char * getName(void);
	const char * getInputFileName(void);
	avlib::ImageType getImageType(void);
	const char * getImageTypeStr(void);
	int getHeight(void);
	int getWidth(void);
protected:
	CDiamondApp(void);
	DiamondOperation parseOperation(const char * op);
	int parseInt(const char * arg);
	avlib::ImageType parseImageType(const char * arg);

	DiamondOperation m_op;
	const char * m_appName;
	const char * m_inputFileName;
	std::string m_shortOpts;
	avlib::ImageType m_imageType;
	const char * m_imageTypeStr;
	int m_imageHeight;
	int m_imageWidth;
};

}
#endif //_DIAMOND_APP_H

