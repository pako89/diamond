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
	try
	{
		app->ParseArgs(argc, argv);
		dbg("Input file : %s\n", app->getInputFileName());
		dbg("Output file: %s\n", app->getOutputFileName());
		dbg("Image type : %s\n", app->getImageTypeStr());
		dbg("Height     : %d\n", app->getHeight());
		dbg("Width      : %d\n", app->getWidth());
		dbg("OpenCL     : %s\n", app->UseOpenCL()?"True":"False");
		diamond::DiamondOperation op = app->getOperation();
		if(diamond::DIAMOND_OP_ENCODE == op)
		{
			avlib::CSequence * seq = new avlib::CSequence(
				app->getInputFile(),
				app->getImageType(),
				app->getHeight(),
				app->getWidth()
			);
			avlib::CBitstream * bstr = new avlib::CBitstream(10000000);
			bstr->set_fh(app->getOutputFile());
			avlib::CBasicEncoder * enc = NULL;
			if(app->UseOpenCL())
			{
				enc = new avlib::CCLEncoder();
			}
			else
			{
		       		enc = new avlib::CBasicEncoder();
			}
			enc->Encode(seq, bstr);
			bstr->flush_all();
			delete enc;
			delete bstr;
			delete seq;
		}
		else if (diamond::DIAMOND_OP_DECODE == op)
		{
			avlib::CSequence seq(app->getOutputFile());
			avlib::CBitstream * bstr = new avlib::CBitstream(10000000);
			bstr->set_fh_fill(app->getInputFile());
			avlib::CBasicDecoder dec;
			dec.Decode(bstr, &seq);
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
