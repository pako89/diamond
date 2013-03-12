#include <stdio.h>
#include <stdlib.h>

#include <DiamondApp.h>
#include <component.h>
#include <huffman.h>
#include <vector>


int main(int argc,char * argv[])
{
	diamond::CDiamondApp * app = diamond::CDiamondApp::getInstance();
	app->setName(argv[0]);
	app->PrintBanner();
#ifdef WIN32
	system("pause");
#endif
	return 0;
}
