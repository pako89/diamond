#include "DiamondApp.h"
#include <string.h>
#include <algorithm>

#ifndef VERSION
#define VERSION	"N/A"
#endif

namespace diamond
{


const char * EncoderVariant2Str(enum EncoderVariant v)
{
	switch(v)
	{
	case ENCODER_VARIANT_CPU:
		return "CPU";
	case ENCODER_VARIANT_OPENCL:
		return "Basic OpenCL";
	case ENCODER_VARIANT_OPENCL_MERGED:
		return "Merged OpenCL";
	case ENCODER_VARIANT_OPENCL_PARALLEL:
		return "Parallel OpenCL";
	default:
		return "Unknown";
	}
}

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

avlib::ImageType CDiamondApp::parseImageType(const char * arg)
{
	avlib::ImageType ret;
	if(NULL != arg)
	{
		ret = avlib::CImageFormat::ParseImageType(arg);
		if(avlib::IMAGE_TYPE_UNKNOWN == ret)
		{
			throw utils::StringFormatException("unknown image type: '%s'", arg);
		}
	}
	else
	{
		throw utils::NullReferenceException();
	}
	return ret;
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

DiamondOperation CDiamondApp::parseOperation(std::string op)
{
	std::transform(op.begin(), op.end(), op.begin(), ::tolower);
	if(op == "encode")
	{
		return diamond::DIAMOND_OP_ENCODE;
	}
	else if(op == "decode")
	{
		return diamond::DIAMOND_OP_DECODE;
	}
	else if(op == "psnr")
	{
		return diamond::DIAMOND_OP_PSNR;
	}
	else
	{
		return diamond::DIAMOND_NOP;
	}
}

void CDiamondApp::PrintVersion(void)
{
	printf("%s version %s\n", m_appName, VERSION);
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
	printf("Usage: %s encode|decode [OPTIONS] FILE\n", m_appName);
}

void CDiamondApp::PrintHelp(void)
{
	PrintBanner();
	PrintUsage();
}


const struct option CDiamondApp::common_options[] = {
	{"help",		no_argument,		NULL,	'h'},
	{"progress-bar",	required_argument, 	NULL, 	'p'},
	{"print-timers",	required_argument,	NULL, 	'T'},
	{"verbose",		no_argument,		NULL,	'v'},
	{"version",		no_argument,		NULL,	'X'},
};

#define COMMON_OPTS_SIZE	ARRAY_SIZE(common_options)

const struct option CDiamondApp::encoder_options[] = {
	{"output",		required_argument,	NULL, 	'o'},
	{"type",		required_argument,	NULL,	't'},
	{"height",		required_argument,	NULL,	'H'},
	{"width",		required_argument,	NULL,	'W'},
	{"variant",		required_argument,	NULL, 	'V'},
	{"huffman",		required_argument, 	NULL, 	'e'},
	{"gop",			required_argument,	NULL, 	'g'},
	{"quant",		required_argument,	NULL, 	'q'},
	{"interpolation",	optional_argument,	NULL,	'I'},
};

#define ENCODER_OPTS_SIZE	ARRAY_SIZE(encoder_options)

const struct option CDiamondApp::decoder_options[] = {
	{"output",		required_argument,	NULL, 	'o'},
};

#define DECODER_OPTS_SIZE	ARRAY_SIZE(decoder_options)

const struct option CDiamondApp::psnr_options[] = {
	{"gop",			required_argument,	NULL, 	'g'},
	{"type",		required_argument,	NULL,	't'},
	{"height",		required_argument,	NULL,	'H'},
	{"width",		required_argument,	NULL,	'W'},
};

#define PSNR_OPTS_SIZE		ARRAY_SIZE(psnr_options)

std::string CDiamondApp::getShortOpts(const struct option long_options[], int size)
{
	std::string shortOpts;
	for(int i=0 ; i<size; i++)
	{
		if(long_options[i].val < 0xf00)
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
	}
	return shortOpts;
}
	
bool CDiamondApp::parseBool(std::string arg)
{
	std::transform(arg.begin(), arg.end(), arg.begin(), ::tolower);
	if(arg == "no" || arg == "false")
	{
		return false;
	}
	else if (arg == "yes" || arg == "true")
	{
		return true;
	}
	else
	{
		throw utils::StringFormatException("invalid value '%s'", optarg);
	}
}

void CDiamondApp::appendLongOptions(std::list<option> & options, const option * long_options, int c)
{
	for(int i=0;i<c;i++)
	{
		options.push_front(long_options[i]);
	}
}

void CDiamondApp::ParseArgs(int argc, char * argv[])
{
	if(argc < 2)
	{
		PrintBanner();
		PrintUsage();
		throw ExitException(0);
	}
	m_config.Op = parseOperation(argv[1]);
	std::list<option> long_options;
	std::string common_opts = getShortOpts(common_options, COMMON_OPTS_SIZE);
	appendLongOptions(long_options, common_options, COMMON_OPTS_SIZE);
	std::string operation_opts;
	switch(m_config.Op)
	{
	case DIAMOND_NOP:
		throw utils::StringFormatException("unknown operation '%s'", argv[1]);
	case DIAMOND_OP_ENCODE:
		operation_opts = getShortOpts(encoder_options, ENCODER_OPTS_SIZE);
		appendLongOptions(long_options, encoder_options, ENCODER_OPTS_SIZE);
		break;
	case DIAMOND_OP_DECODE:
		operation_opts = getShortOpts(decoder_options, DECODER_OPTS_SIZE);
		appendLongOptions(long_options, decoder_options, DECODER_OPTS_SIZE);
		break;
	case DIAMOND_OP_PSNR:
		operation_opts = getShortOpts(psnr_options, PSNR_OPTS_SIZE);
		appendLongOptions(long_options, psnr_options, PSNR_OPTS_SIZE);
		break;
	}
	const char * shortopts = (common_opts + operation_opts).c_str();
	option * _long_options = new option[long_options.size()];
	int i=0;
	for(std::list<option>::iterator itr = long_options.begin(); itr != long_options.end(); ++itr)
	{
		_long_options[i++] = *itr;
	}
	int _argc = argc-1;
	char ** _argv = argv+1;
	int opt, longind;
	while((opt = getopt_long(_argc, _argv, shortopts, _long_options, &longind)) != -1)
	{
		switch(opt)
		{
		case 'h':
			PrintHelp();
			throw ExitException(0);
			break;
		case 'X':
			PrintVersion();
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
			m_config.ImageSize.Width = utils::ParseInt(optarg);
			break;
		case 'H':
			m_config.ImageSize.Height = utils::ParseInt(optarg);
			break;
		case 'V':
			if(optarg)
			{
				m_config.Variant = (EncoderVariant)utils::ParseInt(optarg);
			}
			break;
		case 'e':
			m_config.EncoderConfig.HuffmanType = parseHuffman(optarg);
			break;
		case 'g':
			{
				int gop = utils::ParseInt(optarg);
				if(gop >= 0)
				{
					m_config.EncoderConfig.GOP = gop;
				}
				else
				{
					throw utils::StringFormatException("invalid GOP value: '%d'\n", gop);
				}
			}
			break;
		case 'q':
			{
				int q = utils::ParseInt(optarg);
				if(q>0)
				{
					m_config.EncoderConfig.Q = q;
				}
				else
				{
					throw utils::StringFormatException("invalid Q value: '%d'\n", q);
				}
			}
			break;
		case 'I':
			{
				if(optarg)
				{
					m_config.EncoderConfig.InterpolationScale = utils::ParseInt(optarg);
				}
				else
				{
					m_config.EncoderConfig.InterpolationScale = DEFAULT_INTERPOLATION_SCALE;
				}
			}
			break;
		case 'p':
			m_config.EncoderConfig.PrintProgressBar = parseBool(optarg);
			break;
		case 'T':
			m_config.EncoderConfig.PrintTimers = parseBool(optarg);
			break;
		case 'v':
			inc_logv();
			break;
		case '?':
			break;
		default:
            		throw ParseArgsException("?? getopt returned character code 0%o ??\n", opt);
		}
	}
	delete [] _long_options;
	if(optind < _argc)
	{
		switch(m_config.Op)
		{
		case DIAMOND_OP_ENCODE:
			/* FALL THROUGH */
		case DIAMOND_OP_DECODE:
			m_config.InputFileName = _argv[_argc-1];
			if(strcmp(m_config.InputFileName, "stdin"))
			{
				m_config.InputFile = fopen(m_config.InputFileName, "rb");
				if(NULL == m_config.InputFile)
				{
					throw utils::StringFormatException(strerror(errno));
				}
			}
			break;
		case DIAMOND_OP_PSNR:
			if(optind + 1 < _argc)
			{
				m_config.PSNRConfig.GOP = m_config.EncoderConfig.GOP;
				m_config.PSNRConfig.Seq[0].FileName = _argv[optind];
				m_config.PSNRConfig.Seq[1].FileName = _argv[optind+1];
				if(NULL == (m_config.PSNRConfig.Seq[0].File = fopen(m_config.PSNRConfig.Seq[0].FileName, "rb")))
				{
					throw utils::StringFormatException("can not open file: %s", m_config.PSNRConfig.Seq[0].FileName);
				}
				m_config.PSNRConfig.Seq[1].File = fopen(m_config.PSNRConfig.Seq[1].FileName, "rb");
				if(NULL == (m_config.PSNRConfig.Seq[1].File = fopen(m_config.PSNRConfig.Seq[1].FileName, "rb")))
				{
					throw utils::StringFormatException("can not open file: %s", m_config.PSNRConfig.Seq[1].FileName);
				}
			}
			else
			{
				PrintUsage();
				throw ExitException(1);
			}
			break;
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
