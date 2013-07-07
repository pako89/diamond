#include "DiamondApp.h"
#include <string.h>
#include <algorithm>

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
	m_appName(NULL)
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
	else
	{
		throw utils::NullReferenceException();
	}
}

avlib::HUFFMAN_TYPE CDiamondApp::parseHuffman(const char * arg)
{
	if(!strcmp(arg, "dynamic"))
	{
		return avlib::HUFFMAN_TYPE_DYNAMIC;
	}
	else
	{
		return avlib::HUFFMAN_TYPE_STATIC;
	}
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
		/*TODO*/
	}
	
}

void CDiamondApp::PrintUsage(void)
{
	printf("Usage: %s encode|decode [OPTIONS] FILE\n", "diamond");
}

void CDiamondApp::PrintHelp(void)
{
	PrintBanner();
	PrintUsage();
}


const struct option CDiamondApp::common_options[] = {
	{"help",		no_argument,		NULL,	'h'},
	{"output",		required_argument,	NULL, 	'o'},
	{"type",		required_argument,	NULL,	't'},
	{"height",		required_argument,	NULL,	'H'},
	{"width",		required_argument,	NULL,	'W'},
	{"opencl",		optional_argument,	NULL, 	'C'},
	{"huffman",		required_argument, 	NULL, 	'e'},
	{"gop",			required_argument,	NULL, 	'g'},
#if USE(INTERPOLATION)	
	{"interpolation",	optional_argument,	NULL,	'I'},
#endif
	{"progress-bar",	required_argument, 	NULL, 	'p'}
};

#define COMMON_OPTS_SIZE	ARRAY_SIZE(common_options)

const struct option CDiamondApp::encode_options[] = {
	{}
};

#define ENCODE_OPTS_SIZE	ARRAY_SIZE(encode_options)

const struct option CDiamondApp::decode_options[] = {
	{}
};

#define DECODE_OPTS_SIZE	ARRAY_SIZE(decode_options)

std::string CDiamondApp::getShortOpts(const struct option long_options[], int size)
{
	std::string shortOpts;
	for(int i=0 ; i<size; i++)
	{
		char c = (char)long_options[i].val;
		shortOpts += c;
		if(long_options[i].has_arg == required_argument)
		{
			shortOpts += ':';
		}
		else if(long_options[i].has_arg == optional_argument)
		{
			shortOpts += "::";
		}
	}
	return shortOpts;
}

void CDiamondApp::ParseArgs(int argc, char * argv[])
{
	if(argc < 2)
	{
		PrintBanner();
		PrintUsage();
		throw ExitException(0);
	}
	const char * shortopts = getShortOpts(common_options, COMMON_OPTS_SIZE).c_str();
	m_config.Op = parseOperation(argv[1]);
	const char * op = argv[1];
	
#ifdef WIN32
	int _argc = argc-2;
	char ** _argv = &argv[2];
#else
	int _argc = argc-1;
	char ** _argv = argv+1;
#endif
	int opt, longind;
	while((opt = getopt_long(_argc, _argv, shortopts, CDiamondApp::common_options, &longind)) != -1)
	{
		switch(opt)
		{
		case 'h':
			PrintHelp();
			throw ExitException(0);
			break;
		case 'o':
			m_config.OutputFile = fopen(optarg, "w+");
			if(NULL != m_config.OutputFile)
			{
				m_config.OutputFileName = optarg;
			}
			else
			{
				throw utils::StringFormatException("%s: can not open file for write\n", optarg);
			}
			break;
		case 't':
			m_config.ImageType = parseImageType(optarg);
			m_config.ImageTypeStr = optarg;
			break;
		case 'W':
			m_config.ImageSize.Width = parseInt(optarg);
			break;
		case 'H':
			m_config.ImageSize.Height = parseInt(optarg);
			break;
		case 'C':
			m_config.UseOpenCL = true;
			if(optarg)
			{
				m_config.OpenCLVariant = parseInt(optarg);
			}
			else
			{
				m_config.OpenCLVariant = 1;
			}
			break;
		case 'e':
			m_config.EncoderConfig.HuffmanType = parseHuffman(optarg);
			break;
		case 'g':
			{
				int gop = parseInt(optarg);
				if(gop >= 0)
				{
					m_config.EncoderConfig.GOP = parseInt(optarg);
				}
				else
				{
					throw utils::StringFormatException("wrong GOP: '%d'\n", gop);
				}
			}
			break;
		case 'p':
			{
				std::string pb = optarg;
				std::transform(pb.begin(), pb.end(), pb.begin(), ::tolower);
				if(pb == "no")
				{
					m_config.EncoderConfig.PrintProgressBar = false;
				}
				else if (pb == "yes")
				{
					m_config.EncoderConfig.PrintProgressBar = true;
				}
				else
				{
					throw utils::StringFormatException("invalid value '%s'", optarg);
				}
			}
#if USE(INTERPOLATION)
		case 'I':
			{
				if(optarg)
				{
					m_config.EncoderConfig.InterpolationScale = parseInt(optarg);
				}
				else
				{
					m_config.EncoderConfig.InterpolationScale = DEFAULT_INTERPOLATION_SCALE;
				}
			}
			break;
#endif			
		case '?':
			break;
		default:
            		throw ParseArgsException("?? getopt returned character code 0%o ??\n", opt);
		}
	}
	if(diamond::DIAMOND_NOP == m_config.Op)
	{
		throw utils::StringFormatException("unknown operation '%s'", argv[1]);
	}
	if(optind < _argc)
	{
		m_config.InputFileName = _argv[_argc-1];
		if(strcmp(m_config.InputFileName, "stdin"))
		{
			m_config.InputFile = fopen(m_config.InputFileName, "r");
			if(NULL == m_config.InputFile)
			{
				throw utils::StringFormatException(strerror(errno));
			}
		}
	}
	if(NULL == m_config.InputFileName)
	{
		PrintUsage();
		throw ExitException(1);
	}
}

DiamondConfig CDiamondApp::getConfig(void)
{
	return m_config;
}

}
