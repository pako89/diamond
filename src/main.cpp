#include <stdio.h>
#include <stdlib.h>

#include <DiamondApp.h>
#include <component.h>
#include <huffman.h>
#include <vector>
#include <sequence.h>
#include <basic_encoder.h>
#include <basic_decoder.h>
#include <log.h>



#ifdef WIN32
#define exit(v)		{system("pause"); exit((v));}
#else
#define exit(v)		exit((v));
#endif

int main(int argc,char * argv[])
{
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
		diamond::DiamondOperation op = app->getOperation();
		if(diamond::DIAMOND_OP_ENCODE == op)
		{
			avlib::CSequence seq(
				app->getInputFile(),
				app->getImageType(),
				app->getHeight(),
				app->getWidth()
			);
			avlib::CBitstream bstr(1000000);
			bstr.set_fh(app->getOutputFile());
			avlib::CBasicEncoder enc;
			enc.Encode(&seq, &bstr);
			bstr.flush_all();
		}
		else if (diamond::DIAMOND_OP_DECODE == op)
		{
			avlib::CSequence seq(app->getOutputFile());
			avlib::CBitstream bstr(1000000);
			bstr.set_fh_fill(app->getInputFile());
			avlib::CBasicDecoder dec;
			dec.Decode(&bstr, &seq);
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
