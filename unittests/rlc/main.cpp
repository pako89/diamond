#include <stdio.h>
#include <static_rlc.h>
#include <bitstream.h>
#include <getopt.h>
#include <avlib.h>
#include <ut_common.h>
#include <log.h>

#define BSTR_SIZE	2000000
#define LIMIT(v, l)	((v)>(l)?(l):(v)<-(l)?-(l):(v))
#define LIMIT_DC	2047
#define LIMIT_AC	1023
#define LIMIT_RUN	63
#define RUN_PROB	0.8
#define PROB()		((double)rand()/(double)RAND_MAX)

using namespace avlib;
using namespace utils;


int test_encodeDC(int n, int c)
{
	CStaticRLC<int16_t> * rlc = new CStaticRLC<int16_t>();
	CStaticIRLC<int16_t> * irlc = new CStaticIRLC<int16_t>();
	CBitstream * bstr = new CBitstream(BSTR_SIZE);
	int16_t * tab = new int16_t[n];
	log("Encoding:\n");
	for(int i=0;i<n;i++)
	{
		int t = (rand()%(2*LIMIT_DC))-LIMIT_DC;
		tab[i] = (int16_t)LIMIT(t, LIMIT_DC);
		log("[%03d] encodeDC(%3d, %d)\n", i, tab[i], c);
		rlc->encodeDC(tab[i], c, bstr);
	}
	rlc->Flush(bstr);
	bstr->setPosition(0);
	log("Decoding:\n");
	int counter = 0;
	for(int i=0;i<n;i++)
	{
		int16_t v = irlc->decodeDC(c, bstr);
		bool cmp = (v == tab[i]);
		log("%s[%03d] Decode(%d 0x%x) = %3d [%d 0x%x]\n", cmp?"":"-->", i, c, v, v, tab[i], tab[i]);
		if(v == tab[i]) counter++;
		if(!cmp) exit(1);
	}
	delete [] tab;
	delete bstr;
	delete rlc;
	delete irlc;
	return counter;
}

int test_encodeAC(int n, int c)
{
	int counter = 0;
	CStaticRLC<int16_t> * rlc = new CStaticRLC<int16_t>();
	CStaticIRLC<int16_t> * irlc = new CStaticIRLC<int16_t>();
	CBitstream * bstr = new CBitstream(BSTR_SIZE);
	RunValue<int16_t> * tab = new RunValue<int16_t>[n];
	log("Encoding:\n");
	for(int i=0;i<n;i++)
	{
		int run = rand()%LIMIT_RUN;
		int val;
	       	do { val=(rand()%(2*LIMIT_AC))-LIMIT_AC;} while(val==0);
		tab[i].Run = run;
		tab[i].Value = val;
		log("[%03d] encodeAC(%3d, %2d, %d)\n", i, tab[i].Value, tab[i].Run, c);
		rlc->encodeAC(tab[i].Value, tab[i].Run, c, bstr);
	}
	rlc->Flush(bstr);
	bstr->setPosition(0);
	log("Decoding:\n");
	for(int i=0;i<n;i++)
	{
		RunValue<int16_t> v = irlc->decodeAC(c, bstr);
		bool cmp = (v.Value == tab[i].Value && v.Run == tab[i].Run);
		log("%s[%03d] Decode(%d) = (%2d, %3d) [%2d 0x%x]\n", cmp?"":"-->", i, c, v.Run, v.Value, v.Run, v.Value);
		if(cmp) counter++;
		if(!cmp) exit(1);
	}
	delete [] tab;
	delete bstr;
	delete rlc;
	delete irlc;
	return counter;
}

int test_encodeBlock(int n)
{
	int counter=0;
	CSize s(8, 8);
	int16_t * tab  = new int16_t[s.Width*s.Height];
	int16_t * dtab  = new int16_t[s.Width*s.Height];
	for(int i = 0 ; i < n ; i++)
	{
		CStaticRLC<int16_t> * rlc = new CStaticRLC<int16_t>();
		CStaticIRLC<int16_t> * irlc = new CStaticIRLC<int16_t>();
		CBitstream * bstr = new CBitstream(BSTR_SIZE);
		rlc->Reset();
		irlc->Reset();
		bstr->setPosition(0);
		for(int y = 0 ; y < s.Height; y++)
		{
			for(int x = 0 ; x < s.Width ; x++)
			{
				if(y==0 && x==0)
				{
					int16_t t = (rand()%(2*LIMIT_DC))-LIMIT_DC;
					tab[0] = (int16_t)LIMIT(t, LIMIT_DC); 
				}
				else
				{
					if(PROB() < RUN_PROB)
					{
						tab[y*s.Width+x] = 0;
					}
					else
					{
						int16_t val;
						do { val=(rand()%(2*LIMIT_AC))-LIMIT_AC;} while(val==0);
						tab[y*s.Width+x] = val;
					}
				}
			}
		}
		rlc->EncodeBlock(tab, CPoint(0, 0), s, bstr);
		rlc->Flush(bstr);
		bstr->setPosition(0);
		irlc->DecodeBlock(bstr, dtab, CPoint(0, 0), s);
		bool cmp = true;
		for(int y = 0 ; y < s.Height; y++)
		{
			for(int x = 0 ; x < s.Width ; x++)
			{
				if(tab[y*s.Width+x] != dtab[y*s.Width+x])
				{
					cmp = false;
				}
			}
		}
		if(cmp) counter++;
		delete rlc;
		delete irlc;
		delete bstr;
	}
	delete [] tab;
	delete [] dtab;
	return counter;
}

int main(int argc, char * argv[])
{
	const char * opts = "vn:w:h:";
	int n = 10000;
	int w = 64;
	int h = 64;
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
		case 'w':
			w = atoi(optarg);
			break;
		case 'h':
			h = atoi(optarg);
			break;
		default:
			printf("Unknown option: '%c'\n", opt);
			exit(1);
		}
	}
	CSize s(h, w);
	int r;
	printf("Unit Test - RLC\n");
	printf("[Testing encodeDC...]\n");
	for(int c=0;c<3;c++)
	{
		printf("[c = %d]\n", c);
		r = test_encodeDC(n, c);
		RESULT(r, n);
	}
	printf("[Testing encodeAC...]\n");
	for(int c=0;c<3;c++)
	{
		printf("[c = %d]\n", c);
		r = test_encodeAC(n, c);
		RESULT(r, n);
	}
	printf("[Testing EncodeBlock...]\n");
	r = test_encodeBlock(n);
	RESULT(r, n);
	return 0;
}
