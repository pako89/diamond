#include <stdio.h>
#include <ut_common.h>
#include <bitstream.h>

#define BSTR_SIZE	100000000
using namespace avlib;

int test_position(int n)
{
	int counter = 0;
	CBitstream * bstr = new CBitstream(BSTR_SIZE);
	bstr->putBits(32, 0x12345678);
	bstr->setPosition(4);
	uint8_t d;
	d = bstr->getBits(8);
	printf("0x%x\n", d);
	delete bstr;
	return counter;
}

int main(int argc, char * argv[])
{
	int r;
	int n=1000000;
	printf("Unit Test - Bitstream\n");
	printf("[Testing get/set position...]\n");
	r = test_position(n);
	RESULT(r, n);
	return 0;
}
