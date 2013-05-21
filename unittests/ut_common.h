#ifndef _UT_COMMON_H
#define _UT_COMMON_H

#define RED	"\033[1;91m"
#define EC	"\033[0m"
#define YELLOW	"\033[1;93m"
#define RESULT(r, n)	printf("[%s" EC "] Result: %d/%d\n", (r)==(n)?YELLOW "OK": RED "!!", (r), (n))


#endif // _UT_COMMON_H
