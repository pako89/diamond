#include <stdio.h>
#include <stdlib.h>

#include <DiamondApp.h>

int main(int argc,char * argv[])
{
	diamond::CDiamondApp * app = diamond::CDiamondApp::getInstance();
	app->setName(argv[0]);
	app->PrintBanner();
	return 0;
}
