#include <stdio.h>
#include <stdlib.h>

#include <app.h>
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

int main(int argc,char * argv[])
{
	irena::CApplication * app = irena::CApplication::getInstance();
	app->setName(argv[0]);
	try
	{
		app->ParseArgs(argc, argv);
		irena::Config config = app->getConfig();
		log_prop("Input file", "%s", config.InputFileName);
		log_prop("Output file", "%s", config.OutputFileName);
		if(irena::OP_ENCODE == config.Op)
		{
			const char * huffstr = config.EncoderConfig.HuffmanType==avlib::HUFFMAN_TYPE_DYNAMIC?"dynamic":"static";
			avlib::CSequence * seq = new avlib::CSequence(config.InputFile);
			if(!seq->IsYUV4MPEG())
			{
				seq->setFormat(config.ImageType, config.ImageSize.Height, config.ImageSize.Width);
				seq->setFrameRate(config.FrameRate);
			}
			log_prop("Variant", "%s", irena::EncoderVariant2Str(config.Variant));
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
			case irena::ENCODER_VARIANT_CPU:
		       		enc = new avlib::CBasicEncoder(config.EncoderConfig);
				break;
			case irena::ENCODER_VARIANT_OPENCL:
				enc = new avlib::CCLEncoder(config.EncoderConfig);
				break;
			case irena::ENCODER_VARIANT_OPENCL_PARALLEL:
				enc = new avlib::CCLParallelEncoder(config.EncoderConfig);
				break;
			case irena::ENCODER_VARIANT_OPENCL_MERGED:
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
		else if (irena::OP_DECODE == config.Op)
		{
			avlib::CSequence seq(config.OutputFile);
			avlib::CBitstream * bstr = new avlib::CBitstream(10000000);
			bstr->set_fh_fill(config.InputFile);
			avlib::CBasicDecoder * dec = new avlib::CBasicDecoder(config.EncoderConfig.PrintProgressBar);
			dec->Decode(bstr, &seq);
			delete dec;
			delete bstr;
		}
		else if(irena::OP_PSNR == config.Op)
		{
			if(NULL == config.PSNRConfig.Seq[0].File
			|| NULL == config.PSNRConfig.Seq[1].File)
			{
				throw utils::StringFormatException("You must specify 2 files for PSNR");
			}
			avlib::CSequence * seq1 = new avlib::CSequence(config.PSNRConfig.Seq[0].File);
			if(!seq1->IsYUV4MPEG())
			{
				seq1->setFormat(config.ImageType, config.ImageSize.Height, config.ImageSize.Width);
			}
			avlib::CSequence * seq2 = new avlib::CSequence(config.PSNRConfig.Seq[1].File);
			if(!seq2->IsYUV4MPEG())
			{
				seq2->setFormat(config.ImageType, config.ImageSize.Height, config.ImageSize.Width);
			}
			utils::CPSNR * psnr = new utils::CPSNR(config.PSNRConfig);
			utils::PSNRResults results = psnr->Compute(*seq1, *seq2);
			log_res("Y PSNR", "%2.2f dB", results.PSNR.getMean().Y);
			log_res("U PSNR", "%2.2f dB", results.PSNR.getMean().U);
			log_res("V PSNR", "%2.2f dB", results.PSNR.getMean().V);
			if(config.PSNRConfig.GOP > 1)
			{
				log_res("Y PSNR for I frames", "%2.2f dB", results.I_FRAME_PSNR.getMean().Y);
				log_res("U PSNR for I frames", "%2.2f dB", results.I_FRAME_PSNR.getMean().U);
				log_res("V PSNR for I frames", "%2.2f dB", results.I_FRAME_PSNR.getMean().V);
				log_res("Y PSNR for P frames", "%2.2f dB", results.P_FRAME_PSNR.getMean().Y);
				log_res("U PSNR for P frames", "%2.2f dB", results.P_FRAME_PSNR.getMean().U);
				log_res("V PSNR for P frames", "%2.2f dB", results.P_FRAME_PSNR.getMean().V);
			}
			delete psnr;
			delete seq1;
			delete seq2;
		}
	}
	catch(irena::ExitException & e)
	{
		exit(e.getValue());
	}
	catch(std::exception & e)
	{
		fprintf(stderr, "error: %s\n", e.what());
		exit(1);	
	}
	irena::CApplication::releaseInstance();
	exit(0);
}
