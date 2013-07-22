#include "app.h"
#include <string.h>
#include <algorithm>
#include <cl_base.h>
#include <map>
#include <vector>

#ifndef VERSION
#define VERSION	"N/A"
#endif

#ifndef BUILD_DATE
#define BUILD_DATE __DATE__
#endif

namespace irena
{


const char * EncoderVariant2Str(enum EncoderVariant v)
{
	switch(v)
	{
/* 0 */	case ENCODER_VARIANT_CPU:
		return "CPU";
/* 1 */	case ENCODER_VARIANT_OPENCL:
		return "Basic OpenCL";
/* 2 */	case ENCODER_VARIANT_OPENCL_MERGED:
		return "Merged OpenCL";
/* 3 */	case ENCODER_VARIANT_OPENCL_PARALLEL:
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

CApplication * CApplication::m_instance = NULL;

CApplication::CApplication(void) : 
	m_appName(NULL)
{
}


CApplication::~CApplication(void)
{
	CLOSE(m_config.InputFile);
	CLOSE(m_config.OutputFile);
	CLOSE(m_config.PSNRConfig.Seq[0].File);
	CLOSE(m_config.PSNRConfig.Seq[1].File);
}

CApplication * CApplication::getInstance(void)
{
	if(NULL == m_instance)
	{
		m_instance = new CApplication();
	}
	return m_instance;
}

void CApplication::releaseInstance(void)
{
	if(NULL != m_instance)
	{
		delete m_instance;
		m_instance = NULL;
	}
}

void CApplication::setName(const char * appName)
{
	if(NULL != appName)
	{
		m_appName = appName;
	}
}

const char * CApplication::getName(void)
{
	return m_appName;
}
	
cl_device_type CApplication::parseDevice(std::string dev)
{
	std::transform(dev.begin(), dev.end(), dev.begin(), ::tolower);
	if(dev == "cpu")
	{
		return CL_DEVICE_TYPE_CPU;
	}
	else if(dev == "gpu")
	{
		return CL_DEVICE_TYPE_GPU;
	}
	else
	{
		return CL_DEVICE_TYPE_DEFAULT;
	}
}

avlib::ImageType CApplication::parseImageType(const char * arg)
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

avlib::HUFFMAN_TYPE CApplication::parseHuffman(const char * arg)
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

Operation CApplication::parseOperation(std::string op)
{
	std::transform(op.begin(), op.end(), op.begin(), ::tolower);
	if(op == "encode")
	{
		return irena::OP_ENCODE;
	}
	else if(op == "decode")
	{
		return irena::OP_DECODE;
	}
	else if(op == "psnr")
	{
		return irena::OP_PSNR;
	}
	else if(op == "info")
	{
		return irena::OP_INFO;
	}
	else
	{
		return irena::NOP;
	}
}

void CApplication::PrintVersion(void)
{
	log_info("version", "%s", VERSION);
	log_info("build date", "%s", BUILD_DATE);
	log_info("webpage", "%s", "https://github.com/pako89/irena");
}

void CApplication::PrintBanner(void)
{
	if(NULL != m_appName)
	{
		/*TODO*/
	}
	
}

void CApplication::PrintUsage(void)
{
	printf("Usage: %s encode|decode|psnr|info [OPTIONS] FILE[S]\n", m_appName);
}

void CApplication::PrintHelp(void)
{
printf("\
%s is a non-standarized hybrid video encoder and decoder implementation using CPU and OpenCL technology.\n\
Copyright (c) 2013 by Pawel Lebioda <pawel.lebioda89@gmail.com>\n\
", m_appName);
printf("\n");
	PrintVersion();
printf("\n");
printf("\
Usage: %s OPERATION [OPTIONS] [FILE(S)]\n\
", m_appName);
printf("\n");
printf("\
Avalibale operations:\n\
");
printf("\
	encode	[OPTIONS] FILE	 	Encode sequence using specigied variant of encoder.\n\
			  	  	Input file must be in YUV4MPEG2 format, otherwise sequence\n\
				  	parameters must be specified by encoder options.\n\
");
printf("\
	decode	[OPTIONS] FILE	 	Decode bitstream file. Output file will be in YUV4MPEG2 format\n\
");
printf("\
	psnr	[OPTIONS] FILES	 	Compute PSNR value for two sequences.\n\
				  	Sequences must be in YUV4MPEG2 format, otherwise sequence\n\
				  	parameters must be specified by PSNR options\n\
");
printf("\
	info			 	Print system information including:\n\
				  	* Version\n\
				  	* Build data\n\
				  	* CPU information\n\
				  	* Memory information\n\
				  	* OpenCL platforms and devices informations\n\
");
printf("\n");
printf("\
Common options:\n\
");
printf("\
	-h, --help			Print this help message\n\
");
printf("\
	-v, --verbose			Increase verbose level\n\
");
printf("\
	-X, --version			Print version information\n\
");
printf("\n");
printf("\
Encoder options:\n\
");
printf("\
	-o, --output	FILE		Output file name\n\
					[Default: stdout]\n\
");
printf("\
	-t, --type	FORMAT		Input sequence format type. Valid formats:\n\
					YUV420|420jpeg\n\
");
printf("\
	-H, --height	INT		Input sequence height\n\
");
printf("\
	-W, --width	INT		Input sequence width\n\
");
printf("\
	-V, --variant	INT		Encoder variant. Available variants:\n\
					* 0 - CPU\n\
					* 1 - Basic OpenCL\n\
					* 2 - Merged OpenCL\n\
					* 3 - Parallel OpenCL\n\
");
printf("\
	-g, --gop	INT		Group Of Pictures - number of frames between two succesive I Frames\n\
");
printf("\
	-q, --quant	INT		Quantization coefficient\n\
");
printf("\
	-I, --interpolation INT		Interpolation scale for prediction\n\
");
printf("\
	-k, --kernel	FILE		Path to file contatining kernel sources\n\
");
printf("\
	-d, --device	CPU|GPU		Specify device type for OpenCL\n\
");
printf("\
	-p, --progress-bar yes|no	Indicates whether progress bar should be printed\n\
					Requires -v option\n\
					[Default: no]\n\
");
printf("\
	-T, --print-timers yes|no	Indicates whether timers' values should be printed\n\
					Requires -v option\n\
					[Default: no]\n\
");
printf("\n");
printf("\
Decoder options:\n\
");
printf("\
	-o, --output			Output file name\n\
");
printf("\
	-p, --progress-bar yes|no	Indicates whether progress bar should be printed\n\
					Requires -v option\n\
					[Default: no]\n\
");
printf("\
	-T, --print-timers yes|no	Indicates whether timers' values should be printed\n\
					Requires -v option\n\
					[Default: no]\n\
");
printf("\n");
printf("\
PSNR options:\n\
");
printf("\
	-g, --gop	INT		Group Of Pictures - number of frames between two succesive I Frames\n\
");
printf("\
	-t, --type	FORMAT		Input sequence format type. Valid formats:\n\
					YUV420|420jpeg\n\
");
printf("\
	-H, --height	INT		Input sequence height\n\
");
printf("\
	-W, --width	INT		Input sequence width\n\
");
printf("\n");
}


const struct option CApplication::common_options[] = {
	{"help",		no_argument,		NULL,	'h'},
	{"verbose",		no_argument,		NULL,	'v'},
	{"version",		no_argument,		NULL,	'X'},
};

#define COMMON_OPTS_SIZE	ARRAY_SIZE(common_options)

const struct option CApplication::encoder_options[] = {
	{"output",		required_argument,	NULL, 	'o'},
	{"type",		required_argument,	NULL,	't'},
	{"height",		required_argument,	NULL,	'H'},
	{"width",		required_argument,	NULL,	'W'},
	{"variant",		required_argument,	NULL, 	'V'},
	{"huffman",		required_argument, 	NULL, 	'e'},
	{"gop",			required_argument,	NULL, 	'g'},
	{"quant",		required_argument,	NULL, 	'q'},
	{"interpolation",	optional_argument,	NULL,	'I'},
	{"frame-rate",		required_argument,	NULL,	'f'},
	{"kernel",		required_argument,	NULL,	'k'},
	{"device",		required_argument,	NULL, 	'd'},
	{"progress-bar",	required_argument, 	NULL, 	'p'},
	{"print-timers",	required_argument,	NULL, 	'T'},
};

#define ENCODER_OPTS_SIZE	ARRAY_SIZE(encoder_options)

const struct option CApplication::decoder_options[] = {
	{"output",		required_argument,	NULL, 	'o'},
	{"progress-bar",	required_argument, 	NULL, 	'p'},
	{"print-timers",	required_argument,	NULL, 	'T'},
};

#define DECODER_OPTS_SIZE	ARRAY_SIZE(decoder_options)

const struct option CApplication::psnr_options[] = {
	{"gop",			required_argument,	NULL, 	'g'},
	{"type",		required_argument,	NULL,	't'},
	{"height",		required_argument,	NULL,	'H'},
	{"width",		required_argument,	NULL,	'W'},
};

#define PSNR_OPTS_SIZE		ARRAY_SIZE(psnr_options)

std::string CApplication::getShortOpts(const struct option long_options[], int size)
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
	
bool CApplication::parseBool(std::string arg)
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

void CApplication::appendLongOptions(std::list<option> & options, const option * long_options, int c)
{
	for(int i=0;i<c;i++)
	{
		options.push_front(long_options[i]);
	}
}

void CApplication::ParseArgs(int argc, char * argv[])
{
	if(NULL != argv)
	{
#ifndef WIN32
		m_appName = basename(argv[0]);
#else
		m_appName = argv[0];
#endif
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
	case OP_ENCODE:
		operation_opts = getShortOpts(encoder_options, ENCODER_OPTS_SIZE);
		appendLongOptions(long_options, encoder_options, ENCODER_OPTS_SIZE);
		break;
	case OP_DECODE:
		operation_opts = getShortOpts(decoder_options, DECODER_OPTS_SIZE);
		appendLongOptions(long_options, decoder_options, DECODER_OPTS_SIZE);
		break;
	case OP_PSNR:
		operation_opts = getShortOpts(psnr_options, PSNR_OPTS_SIZE);
		appendLongOptions(long_options, psnr_options, PSNR_OPTS_SIZE);
		break;
	case OP_INFO:
		PrintVersion();
		PrintCPUInfo();
		PrintMemInfo();
		PrintOpenCLInfo();
		throw ExitException(0);
	default:
		break;
	}
	std::string shortopts = common_opts + operation_opts;
	option * _long_options = new option[long_options.size()];
	int i=0;
	for(std::list<option>::iterator itr = long_options.begin(); itr != long_options.end(); ++itr)
	{
		_long_options[i++] = *itr;
	}
	int _argc;
	char ** _argv;
	if(NOP != m_config.Op)
	{
#ifndef WIN32
		_argc = argc-1;
		_argv = argv+1;
#else
		_argc = argc-2;
		_argv = argv+2;
#endif
	}
	else
	{
		_argc = argc;
		_argv = argv;
	}
	int opt, longind;
	while((opt = getopt_long(_argc, _argv, shortopts.c_str(), _long_options, &longind)) != -1)
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
			m_config.OutputFile = fopen(optarg, "wb");
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
		case 'k':
			m_config.EncoderConfig.KernelSrc = optarg;
			break;
		case 'd':
			m_config.EncoderConfig.Device = parseDevice(optarg);
			if(CL_DEVICE_TYPE_DEFAULT == m_config.EncoderConfig.Device)
			{
				throw utils::StringFormatException("unknown device type: '%s'", optarg);
			}
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
	if(NOP == m_config.Op)
	{
		throw utils::StringFormatException("unknown operation '%s'", argv[1]);
	}
	if(optind < _argc)
	{
		switch(m_config.Op)
		{
		case OP_ENCODE:
			/* FALL THROUGH */
		case OP_DECODE:
			m_config.InputFileName = _argv[_argc-1];
			if(strcmp(m_config.InputFileName, "stdin"))
			{
				m_config.InputFile = fopen(m_config.InputFileName, "rb");
				if(NULL == m_config.InputFile)
				{
					throw utils::StringFormatException("%s: no such file", m_config.InputFileName);
				}
			}
			break;
		case OP_PSNR:
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
		default:
			break;
		}
	}
	if(NULL == m_config.InputFileName)
	{
		PrintUsage();
		throw ExitException(1);
	}
}

Config CApplication::getConfig(void)
{
	return m_config;
}

CApplication::props_t CApplication::GetProcInfo(const char * procname)
{
#define BUFF_SIZE	4096
	static char BUFF[BUFF_SIZE];
	FILE * fh = fopen(procname, "r");
	if(NULL == fh)
	{
		throw utils::StringFormatException("can not open /proc/cpuinfo for read");
	}
	CApplication::props_t l;
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

void CApplication::PrintMemInfo(void)
{
#ifndef WIN32
	CApplication::props_t props = CApplication::GetProcInfo("/proc/meminfo");
	if(props.size() > 0)
	{
		logv(0, "** Memory Info\n\n");
		for(CApplication::props_t::iterator itr = props.begin(); itr != props.end(); ++itr)
		{
			log_info((*itr).first.c_str(), "%s", (*itr).second.c_str());
		}
		logv(0, "\n");
	}
#endif
}	

void CApplication::PrintCPUInfo(void)
{
#ifndef WIN32
	CApplication::props_t props = CApplication::GetProcInfo("/proc/cpuinfo");
	if(props.size() > 0)
	{
		logv(0, "\n** CPU Info\n");
		for(CApplication::props_t::iterator itr = props.begin(); itr != props.end(); ++itr)
		{
			if((*itr).first == "processor")
			{
				logv(0, "\n");
			}
			log_info((*itr).first.c_str(), "%s", (*itr).second.c_str());
		}
		logv(0, "\n");
	}
#endif
}

void CApplication::PrintOpenCLInfo(void)
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
		log_info("Platform", "%d", i);
		log_info("Name", "%s", info->getName().c_str());
		log_info("Vendor", "%s", info->getVendor().c_str());
		log_info("Version", "%s", info->getVersion().c_str());
		log_info("Profile", "%s", info->getProfile().c_str());
		std::vector<std::string> extensions;
		utils::split(info->getExtensions().c_str(), ' ', extensions);
		if(extensions.size() > 0)
		{
			log_info("Extensions", "%s", extensions[0].c_str());
			for(unsigned int i=1;i<extensions.size();i++)
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
			log_info("Device", "%d", j);
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
				for(unsigned int i=1;i<sizes.size();i++)
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
				for(unsigned int i=1;i<extensions.size();i++)
				{
					if(extensions[i].size() > 0 && extensions[i] != " ")
					{
						log_info("", "%s", extensions[i].c_str());
					}
				}
			}
		}
	}
}

}
