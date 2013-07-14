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
	try
	{
		app->ParseArgs(argc, argv);
		diamond::DiamondConfig config = app->getConfig();
		log_prop("Input file", "%s", config.InputFileName);
		log_prop("Output file", "%s", config.OutputFileName);
		if(diamond::DIAMOND_OP_ENCODE == config.Op)
		{
			const char * huffstr = config.EncoderConfig.HuffmanType==avlib::HUFFMAN_TYPE_DYNAMIC?"dynamic":"static";
			avlib::CSequence * seq = new avlib::CSequence(config.InputFile);
			if(!seq->IsYUV4MPEG())
			{
				seq->setFormat(config.ImageType, config.ImageSize.Height, config.ImageSize.Width);
			}
			log_prop("Variant", "%s", diamond::EncoderVariant2Str(config.Variant));
			log_prop("Image type", "%s", config.ImageTypeStr);
			log_prop("Huffman", "%s", huffstr);
			log_prop("GOP", "%d", config.EncoderConfig.GOP);
			log_prop("Quant coef", "%d", config.EncoderConfig.Q);
			log_prop("Interpolation scale", "%d", config.EncoderConfig.InterpolationScale);
			log_prop("Width", "%d", seq->getFormat().Size.Width);
			log_prop("Height", "%d", seq->getFormat().Size.Height);
			log_prop("Frame rate", "%d:%d", seq->getFrameRate().Nom, seq->getFrameRate().Denom);
			log_prop("Numer of frames", "%d", seq->getFramesCount());
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
