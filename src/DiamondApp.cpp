#include "DiamondApp.h"
#include <string.h>

#define BANNER									"\
     _ _                                 _	\n\
  __| (_) __ _ _ __ ___   ___  _ __   __| |	\n\
 / _` | |/ _` | '_ ` _ \ / _ \| '_ \ / _` | \n\
| (_| | | (_| | | | | | | (_) | | | | (_| | \n\
 \__,_|_|\__,_|_| |_| |_|\___/|_| |_|\__,_| \n"

namespace diamond
{

ExitException::ExitException() :
	m_val(0)
{
}

ExitException::ExitException(int ret) :
	m_val(ret)
{
}

int ExitException::getValue(void) const throw()
{
	return m_val;
}

ParseArgsException::ParseArgsException(const char * fmt, ...)
{
	va_list argptr;
	va_start(argptr, fmt);
	m_msg = utils::string_format(fmt, argptr);
	va_end(argptr);
}

CDiamondApp * CDiamondApp::m_instance = NULL;

CDiamondApp::CDiamondApp(void) : 
	m_op(DIAMOND_NOP),
	m_appName(NULL),
	m_inputFileName(NULL),
	m_imageType(avlib::IMAGE_TYPE_UNKNOWN),
	m_imageTypeStr("unknown"),
	m_imageHeight(0),
	m_imageWidth(0)
{
}


CDiamondApp::~CDiamondApp(void)
{
}

CDiamondApp * CDiamondApp::getInstance(void)
{
	if(NULL == m_instance)
	{
		m_instance = new CDiamondApp();
	}
	return m_instance;
}

void CDiamondApp::setName(const char * appName)
{
	if(NULL != appName)
	{
		m_appName = appName;
	}
}

const char * CDiamondApp::getName(void)
{
	return m_appName;
}

int CDiamondApp::parseInt(const char * arg)
{
	if(NULL != arg)
	{
		int ret = atoi(arg);
		if(ret == 0 && arg[0] != '0')
		{
			throw utils::StringFormatException("can not convert '%s' to integer", arg);
		}
		return ret;
	}
	else
	{
		throw utils::NullReferenceException();
	}
}

avlib::ImageType CDiamondApp::parseImageType(const char * arg)
{
	if(NULL != arg)
	{
		if(!strcmp("YUV420", arg))
		{
			return avlib::IMAGE_TYPE_YUV420;
		}
		else
		{
			throw utils::StringFormatException("unknown image type: '%s'", arg);
		}
	}
	throw utils::NullReferenceException();
}

DiamondOperation CDiamondApp::parseOperation(const char * op)
{
	if(!strcmp(op, "encode"))
	{
		return diamond::DIAMOND_OP_ENCODE;
	}
	else if(!strcmp(op, "decode"))
	{
		return diamond::DIAMOND_OP_DECODE;
	}
	else
	{
		return diamond::DIAMOND_NOP;
	}
}

void CDiamondApp::PrintBanner(void)
{
	if(NULL != m_appName)
	{
		//printf("%s\n", BANNER);
	}
	
}

void CDiamondApp::PrintUsage(void)
{
	printf("Usage: %s encode|decode [-%s] FILE\n", "diamond", m_shortOpts.c_str());
}

void CDiamondApp::PrintHelp(void)
{
	PrintBanner();
	PrintUsage();
}

const struct option CDiamondApp::long_options[] = {
	{"help",		no_argument,		NULL,	'h'},
	{"type",		required_argument,	NULL,	't'},
	{"height",		required_argument,	NULL,	'H'},
	{"width",		required_argument,	NULL,	'W'},	
};

#define LONG_OPTS_SIZE()	(sizeof(long_options)/sizeof(long_options[0]))

void CDiamondApp::ParseArgs(int argc, char * argv[])
{
	for(int i=0 ; i<LONG_OPTS_SIZE(); i++)
	{
		char c = (char)CDiamondApp::long_options[i].val;
		m_shortOpts += c;
		if(CDiamondApp::long_options[i].has_arg == required_argument)
		{
			m_shortOpts += ':';
		}
	}
	const char * shortopts = m_shortOpts.c_str();
	if(argc < 3)
	{
		PrintBanner();
		PrintUsage();
		throw ExitException(0);
	}
	if(diamond::DIAMOND_NOP == (m_op = parseOperation(argv[1])))
	{
		throw utils::StringFormatException("unknown operation '%s'", argv[1]);
	}
	argv+=1;
	argc-=1;
	int opt, longind;
	while((opt = getopt_long(argc, argv, shortopts, CDiamondApp::long_options, &longind)) != -1)
	{
		switch(opt)
		{
		case 'h':
			PrintHelp();
			throw ExitException(0);
			break;
		case 't':
			m_imageType = parseImageType(optarg);
			m_imageTypeStr = optarg;
			break;
		case 'W':
			m_imageWidth = parseInt(optarg);
			break;
		case 'H':
			m_imageHeight = parseInt(optarg);
			break;
		case '?':
			break;
		default:
            throw ParseArgsException("?? getopt returned character code 0%o ??\n", opt);
		}
	}
	if(optind < argc)
	{
		m_inputFileName = argv[optind];
	}
	if(NULL == m_inputFileName)
	{
		PrintUsage();
		throw ExitException(1);
	}
}

const char * CDiamondApp::getInputFileName(void)
{
	return m_inputFileName;
}

avlib::ImageType CDiamondApp::getImageType(void)
{
	return m_imageType;
}

const char * CDiamondApp::getImageTypeStr(void)
{
	return m_imageTypeStr;
}

int CDiamondApp::getHeight(void)
{
	return m_imageHeight;
}

int CDiamondApp::getWidth(void)
{
	return m_imageWidth;
}


}
