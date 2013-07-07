#include <stdio.h>
#include <stdlib.h>

#include <DiamondApp.h>
#include <component.h>
#include <dynamic_huffman.h>
#include <vector>
#include <sequence.h>
#include <basic_encoder.h>
#include <basic_decoder.h>
#include <cl_encoder.h>
#include <cl_parallel_encoder.h>
#include <cl_merged_encoder.h>
#include <log.h>

module("Main");

#ifdef WIN32
#define exit(v)		{system("pause"); exit((v));}
#else
#define exit(v)		exit((v));
#endif

int main(int argc,char * argv[])
{
	avlib::CComponent<uint8_t> u8;
	avlib::CComponent<float> fl;
	fl=u8;
	diamond::CDiamondApp * app = diamond::CDiamondApp::getInstance();
	app->setName(argv[0]);
	inc_logv();
	try
	{
		app->ParseArgs(argc, argv);
		diamond::DiamondConfig config = app->getConfig();
		dbg("Input file : %s\n", config.InputFileName);
		dbg("Output file: %s\n", config.OutputFileName);
		dbg("Image type : %s\n", config.ImageTypeStr);
		dbg("Height     : %d\n", config.ImageSize.Height);
		dbg("Width      : %d\n", config.ImageSize.Width);
		dbg("Variant    : %s\n", diamond::EncoderVariant2Str(config.Variant));
		if(diamond::DIAMOND_OP_ENCODE == config.Op)
		{
			dbg("Huffman    : %s\n", config.EncoderConfig.HuffmanType==avlib::HUFFMAN_TYPE_DYNAMIC?"dynamic":"static");
			dbg("GOP        : %d\n", config.EncoderConfig.GOP);
			avlib::CSequence * seq = new avlib::CSequence(
				config.InputFile,
				config.ImageType,
				config.ImageSize.Height,
				config.ImageSize.Width
			);
			avlib::CBitstream * bstr = new avlib::CBitstream(10000000);
			bstr->set_fh(config.OutputFile);
			avlib::CEncoder * enc = NULL;
			switch(config.Variant)
			{
			case diamond::ENCODER_VARIANT_CPU:
		       		enc = new avlib::CBasicEncoder(config.EncoderConfig);
				break;
			case diamond::ENCODER_VARIANT_OPENCL:
				enc = new avlib::CCLEncoder(config.EncoderConfig);
				break;
			case diamond::ENCODER_VARIANT_OPENCL_PARALLEL:
				enc = new avlib::CCLParallelEncoder(config.EncoderConfig);
				break;
			case diamond::ENCODER_VARIANT_OPENCL_MERGED:
				enc = new avlib::CCLMergedEncoder(config.EncoderConfig);
				break;
			default:
				throw utils::StringFormatException("Unknown OpenCL variant: '%d'", config.Variant);
			}
			enc->Encode(seq, bstr);
			bstr->flush_all();
			delete enc;
			delete bstr;
			delete seq;
		}
		else if (diamond::DIAMOND_OP_DECODE == config.Op)
		{
			avlib::CSequence seq(config.OutputFile);
			avlib::CBitstream * bstr = new avlib::CBitstream(10000000);
			bstr->set_fh_fill(config.InputFile);
			avlib::CBasicDecoder * dec = new avlib::CBasicDecoder(config.EncoderConfig.PrintProgressBar);
			dec->Decode(bstr, &seq);
			delete dec;
			delete bstr;
		}
	}
	catch(diamond::ExitException & e)
	{
		exit(e.getValue());
	}
	catch(std::exception & e)
	{
		fprintf(stderr, "error: %s\n", e.what());
		exit(1);	
	}
	
	exit(0);
}
