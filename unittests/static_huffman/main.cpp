#include <stdio.h>
#include <static_huffman.h>
#include <bitstream.h>
#include <log.h>
#include <getopt.h>
#include <ut_common.h>

using namespace avlib;
#define BSTR_SIZE	20000000
int test_put_get_1b(int n)
{
	CStaticHuffman<uint32_t> * sh = new CStaticHuffman<uint32_t>();
	CBitstream * bstr = new CBitstream(BSTR_SIZE);
	uint32_t * tab  = new uint32_t[n];
	int * nbits = new int[n];
	log("Encoding:\n");
	for(int i = 0 ; i < n ; i++)
	{
		tab[i] = rand();
		nbits[i] = 1;
		tab[i] = tab[i]&((1<<nbits[i])-1);
		log("[%03d] Insert %d bits: 0x%x\n", i, nbits[i], tab[i]&((1<<nbits[i])-1));
		sh->Put(tab[i], nbits[i], bstr);
	}
	sh->Flush(bstr);
	bstr->setPosition(0);
	int counter = 0;
	log("Decoding:\n");
	for(int i = 0 ; i < n ; i++)
	{
		int v = sh->Get(nbits[i], bstr);
		bool cmp = (v == tab[i]);
		if(cmp)
			counter++;
		log("%s[%03d] Get %d bits: 0x%x [0x%x]\n",cmp?"":"-->", i, nbits[i], v, tab[i]);
	}
	delete [] tab;
	delete [] nbits;
	delete bstr;
	delete sh;
	return counter;
}

int test_put_get(int n)
{
	CStaticHuffman<uint32_t> * sh = new CStaticHuffman<uint32_t>();
	CBitstream * bstr = new CBitstream(BSTR_SIZE);
	uint32_t * tab  = new uint32_t[n];
	int * nbits = new int[n];
	log("Encoding:\n");
	for(int i = 0 ; i < n ; i++)
	{
		tab[i] = rand();
		nbits[i] = rand()%32;
		tab[i] = tab[i]&((1<<nbits[i])-1);
		log("[%03d] Insert %d bits: 0x%x\n", i, nbits[i], tab[i]&((1<<nbits[i])-1));
		sh->Put(tab[i], nbits[i], bstr);
	}
	sh->Flush(bstr);
	bstr->setPosition(0);
	int counter = 0;
	log("Decoding:\n");
	for(int i = 0 ; i < n ; i++)
	{
		int v = sh->Get(nbits[i], bstr);
		bool cmp = (v == tab[i]);
		if(cmp)
			counter++;
		log("%s[%03d] Get %d bits: 0x%x [0x%x]\n",cmp?"":"-->", i, nbits[i], v, tab[i]);
	}
	delete [] tab;
	delete [] nbits;
	delete bstr;
	delete sh;
	return counter;
}

int test_enc_dec(int n, bool dc, int c)
{
	CStaticHuffman<uint32_t> * sh = new CStaticHuffman<uint32_t>();
	CBitstream * bstr = new CBitstream(BSTR_SIZE);
	huffman_code_t * tab = new huffman_code_t[n];
	log("Encoding:\n");
	for(int i = 0 ; i < n ; i++)
	{
		huffman_code_t s;
		do
		{
			s=(huffman_code_t)(rand());
		}while(!sh->Contains(s, dc, c));
		tab[i] = s; 
		log("[%03d] Encode(0x%02x)\n", i, tab[i]);
		sh->Encode(tab[i], bstr, dc, c);
	}
	sh->Flush(bstr);
	bstr->setPosition(0);
	log("Decoding:\n");
	int counter = 0;
	for(int i = 0 ; i < n ; i++)
	{
		huffman_code_t v =  sh->Decode(bstr, dc, c);
		bool cmp = (v == tab[i]);
		if(cmp) counter++;
		log("%s[%03d] Decode(0x%02x) [0x%02x]\n", cmp?"":"-->", i, v, tab[i]);
		if(!cmp) exit(1);
	}
	delete [] tab;
	delete bstr;
	delete sh;
	return counter;
}

const char * opts = "vn:";


int main(int argc, char * argv[])
{
	int n = 10000;
	if(argc < 2)
	{
		printf("Usage: %s [-v - verbose][-n INT - Number of samples]\n", argv[0]);
		exit(0);
	}
	int opt;
	while((opt=getopt(argc, argv, opts)) != -1)
	{
		switch(opt)
		{
		case 'v':
			inc_logv();
			break;
		case 'n':
			n = atoi(optarg);
			break;
		default:
			printf("Unknown option: '%c'\n", opt);
			exit(1);
		}
	}
	printf("Unit Test - Static Huffman\n");
	int r;
	printf("[Testing Put() and Get()...]\n");
	r = test_put_get(n);
	RESULT(r, n);
	printf("[Testing Put() and Get() 1 bit...]\n");
	r = test_put_get_1b(n);
	RESULT(r, n);
	printf("[Testing Encode() and Decode()...]\n");
	for(int dc=1;dc>=0;dc--)
	{
		for(int c=0;c<3;c++)
		{
			printf("[dc = %s, c = %d]\n", dc?"True ":"False", c);
			r = test_enc_dec(n, dc, c);
			RESULT(r, n);
		}
	}
	return 0;
}


