#include "DiamondApp.h"
#include <string.h>
#include <algorithm>
#include <cl_base.h>
#include <map>
#include <vector>

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
	else if(op == "info")
	{
		return diamond::DIAMOND_OP_INFO;
	}
	else
	{
		return diamond::DIAMOND_NOP;
	}
}

void CDiamondApp::PrintVersion(void)
{
	printf("version %s\n", VERSION);
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
	printf("Usage: %s encode|decode|psnr [OPTIONS] FILE[S]\n", m_appName);
}

void CDiamondApp::PrintHelp(void)
{
	PrintBanner();
	PrintUsage();
	PrintVersion();
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
	if(NULL != argv)
	{
		m_appName = basename(argv[0]);
	}
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
	case DIAMOND_OP_INFO:
		PrintCPUInfo();
		PrintMemInfo();
		PrintOpenCLInfo();
		throw ExitException(0);
	}
	const char * shortopts = (common_opts + operation_opts).c_str();
	option * _long_options = new option[long_options.size()];
	int i=0;
	for(std::list<option>::iterator itr = long_options.begin(); itr != long_options.end(); ++itr)
	{
		_long_options[i++] = *itr;
	}
	int _argc;
	char ** _argv;
	if(DIAMOND_NOP != m_config.Op)
	{
		_argc = argc-1;
		_argv = argv+1;
	}
	else
	{
		_argc = argc;
		_argv = argv;
	}
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
	if(DIAMOND_NOP == m_config.Op)
	{
		throw utils::StringFormatException("unknown operation '%s'", argv[1]);
	}
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

CDiamondApp::props_t CDiamondApp::GetProcInfo(const char * procname)
{
#define BUFF_SIZE	4096
	static char BUFF[BUFF_SIZE];
	FILE * fh = fopen(procname, "r");
	if(NULL == fh)
	{
		throw utils::StringFormatException("can not open /proc/cpuinfo for read");
	}
	CDiamondApp::props_t l;
	char * line = NULL;
	while(NULL != (line = fgets(BUFF, BUFF_SIZE, fh)))
	{
		std::vector<std::string> p;
		std::string s = std::string(line);
		s.resize(s.size()-1);
		utils::split(s, ':', p);
		if(p.size() == 2)
		{
			std::string n = utils::trim(p[0]);
			std::string v = utils::trim(p[1]);
			std::pair<std::string, std::string> pair(n, v);
			l.push_back(pair);
		}
	}
	fclose(fh);
	return l;
}

void CDiamondApp::PrintMemInfo(void)
{
	CDiamondApp::props_t props = CDiamondApp::GetProcInfo("/proc/meminfo");
	if(props.size() > 0)
	{
		logv(0, "** Memory Info\n\n");
		for(CDiamondApp::props_t::iterator itr = props.begin(); itr != props.end(); ++itr)
		{
			log_info((*itr).first.c_str(), "%s", (*itr).second.c_str());
		}
		logv(0, "\n");
	}
}	

void CDiamondApp::PrintCPUInfo(void)
{
	CDiamondApp::props_t props = CDiamondApp::GetProcInfo("/proc/cpuinfo");
	if(props.size() > 0)
	{
		logv(0, "** CPU Info\n");
		for(CDiamondApp::props_t::iterator itr = props.begin(); itr != props.end(); ++itr)
		{
			if((*itr).first == "processor")
			{
				logv(0, "\n");
			}
			log_info((*itr).first.c_str(), "%s", (*itr).second.c_str());
		}
		logv(0, "\n");
	}
}

void CDiamondApp::PrintOpenCLInfo(void)
{
	CCLBase & base = CCLBase::getInstance();
	logv(0, "** OpenCL platforms information\n\n");
	log_info("Number of platforms", "%d", base.getPlatformsCount());
	int n = base.getPlatformsCount();
	for(int i=0;i<n; i++)
	{
		CCLPlatformInfo * info = base.getPlatform(i)->getInfo();
		if(NULL == info)
		{
			throw utils::StringFormatException("can not get platform info");
		}
		logv(0, "\n");
		logv(0, "** Platform [%d]\n", i+1);
		log_info("Name", "%s", info->getName().c_str());
		log_info("Vendor", "%s", info->getVendor().c_str());
		log_info("Version", "%s", info->getVersion().c_str());
		log_info("Profile", "%s", info->getProfile().c_str());
		std::vector<std::string> extensions;
		utils::split(info->getExtensions().c_str(), ' ', extensions);
		if(extensions.size() > 0)
		{
			log_info("Extensions", "%s", extensions[0].c_str());
			for(int i=1;i<extensions.size();i++)
			{
				log_info("", "%s", extensions[i].c_str());
			}
		}
		log_info("Number of devices", "%d", base.getPlatform(i)->getDevicesCount());
		int m = base.getPlatform(i)->getDevicesCount();
		for(int j=0;j<m;j++)
		{
			CCLDeviceInfo * dinfo = base.getPlatform(i)->getDevicePtr(j)->getInfo();
			if(NULL == dinfo)
			{
				throw utils::StringFormatException("can no get device info");
			}
			logv(0, "\n");
			logv(0, "** Device [%d]\n", j+1);
			log_sep(0);
			log_info("Name", "%s", dinfo->getName().c_str());
			log_info("Profile", "%s", dinfo->getProfile().c_str());
			log_info("Device Version", "%s", dinfo->getDeviceVersion().c_str());
			log_info("Driver Version", "%s", dinfo->getDriverVersion().c_str());
			log_info("Vendor", "%s", dinfo->getVendor().c_str());
			log_info("Vendor ID", "%d", dinfo->getVendorId());
			log_info("Global memory size", "%llu", dinfo->getGlobalMemSize());
			log_info("Global memory cache size", "%llu", dinfo->getGlobalMemCacheSize());
			log_info("Local memory size", "%llu", dinfo->getLocalMemSize());
			log_info("Max const buffer size", "%llu", dinfo->getMaxConstBuffSize());
			log_info("Max memory alloc size", "%llu", dinfo->getMaxMemAllocSize());
			log_info("Address bits", "%d", dinfo->getAddressBits());
			log_info("Device available", "%s", utils::bool2str(dinfo->getAvailable()).c_str());
			log_info("Compiler available", "%s", utils::bool2str(dinfo->getCompilerAvailable()).c_str());
			log_info("Endian little", "%s", utils::bool2str(dinfo->getEndianLittle()).c_str());
			log_info("Error correction support", "%s", utils::bool2str(dinfo->getErrorCorrectionSupport()).c_str());
			log_info("Global memory cache line size", "%u", dinfo->getGlobalMemCacheLineSize());
			log_info("Image support", "%s", utils::bool2str(dinfo->getImageSupport()).c_str());
			log_info("Image 2D max height", "%d", dinfo->getImage2DMaxHeight());
			log_info("Image 2D max width", "%d", dinfo->getImage2DMaxWidth());
			log_info("Image 3D max depth", "%d", dinfo->getImage3DMaxDepth());
			log_info("Image 3D max height", "%d", dinfo->getImage3DMaxHeight());
			log_info("Image 3D max width", "%d", dinfo->getImage3DMaxWidth());
			log_info("Max clock frequency", "%d", dinfo->getMaxClockFrequency());
			log_info("Max compute units", "%d", dinfo->getMaxComputeUnits());
			log_info("Max constant args", "%d", dinfo->getMaxConstantArgs());
			log_info("Max parameter size", "%d", dinfo->getMaxParameterSize());
			log_info("Max read image args", "%d", dinfo->getMaxReadImageArgs());
			log_info("Max write image args", "%d", dinfo->getMaxWriteImageArgs());
			log_info("Max samplers", "%d", dinfo->getMaxSamplers());
			log_info("Max work group size", "%d", dinfo->getMaxWorkGroupSize());
			log_info("Max work item dimensions", "%d", dinfo->getMaxWorkItemDimensions());
			std::vector<size_t> sizes = dinfo->getMaxWorkItemSizes();
			if(sizes.size()>0)
			{
				log_info("Max work item sizes", "%d", sizes[0]);
				for(int i=1;i<sizes.size();i++)
				{
					log_info("", "%d", sizes[i]);
				}
				
			}
			log_info("Memory base address align", "%d", dinfo->getMemBaseAddrAlign());
			log_info("Min data type align size", "%d", dinfo->getMinDataTypeAlignSize());
			log_info("Preffered vector width char", "%d", dinfo->getPrefferedVectorWidthChar());
			log_info("Preffered vector width short", "%d", dinfo->getPrefferedVectorWidthShort());
			log_info("Preffered vector width int", "%d", dinfo->getPrefferedVectorWidthInt());
			log_info("Preffered vector width long", "%d", dinfo->getPrefferedVectorWidthLong());
			log_info("Preffered vector width float", "%d", dinfo->getPrefferedVectorWidthFloat());
			log_info("Preffered vector width double", "%d", dinfo->getPrefferedVectorWidthDouble());
			log_info("Profiling timer resolution", "%d", dinfo->getProfilingTimerResolution());
			std::vector<std::string> extensions;
			utils::split(dinfo->getExtensions().c_str(), ' ', extensions);
			if(extensions.size() > 0)
			{
				log_info("Extensions", "%s", extensions[0].c_str());
				for(int i=1;i<extensions.size();i++)
				{
					if(extensions[i].size() > 0 && extensions[i] != " ")
					{
						log_info("", "%s", extensions[i].c_str());
					}
				}
			}
			log_sep(0);
		}
	}
}

}
