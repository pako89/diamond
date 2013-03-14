#include <stdio.h>
#include <stdlib.h>

#include <DiamondApp.h>
#include <component.h>
#include <huffman.h>
#include <vector>
#include <sequence.h>

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
		printf("Input file: %s\n", app->getInputFileName());
		printf("Image type: %s\n", app->getImageTypeStr());
		printf("Height    : %d\n", app->getHeight());
		printf("Width     : %d\n", app->getWidth());
		avlib::CBitstream bstr(100000);
		FILE * fh = fopen("output.diamond", "w+");
		if(NULL == fh)
		{
			perror("fopen");
			exit(1);
		}
		bstr.set_fh(fh);
		avlib::CHuffmanTree<uint8_t> htree;
		avlib::CSequence seq;
		seq.OpenFile(
			app->getInputFileName(),
			app->getImageType(),
			app->getHeight(),
			app->getWidth()
			);
		do
		{
			for(int k = 0 ; k < seq.getFrame().getComponents(); k++)
			{
				for(int y=0;y<seq.getFrame()[k].getHeight(); y++)
				{
					for(int x=0;x<seq.getFrame()[k].getWidth(); x++)
					{
						htree.Encode(seq.getFrame()[k][y][x], &bstr);
					}
				}
				bstr.flush();
			}
		}while(seq.Next());		
		bstr.flush_all();
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
