#ifndef _LOG_H
#define _LOG_H

#include <stdio.h>

#define RED	"\033[1;91m"
#define EC	"\033[0m"
#define YELLOW	"\033[1;93m"

extern int logv;

#ifdef DUMP_BITSTREAM
extern FILE * fh_w;
extern FILE * fh_r;
#endif

#define _FILE			stderr
#define _LOG(...)		fprintf(_FILE, __VA_ARGS__)

#define inc_logv()		(logv++)
#define logv(...)		if(logv>0) _LOG(__VA_ARGS__)
#define log_prop(n, f, ...)	logv("%-30s : " f "\n", n, __VA_ARGS__)
#define log_timer(n, t)		log_prop("Timer " n, "%.2f", (t).getTotalSeconds())
//logv("Timer %-14s : %f\n", (name), (timer).getTotalSeconds())
#ifdef DEBUG

#define dbg(...)		_LOG(__VA_ARGS__)
#define dbgf(...)		_LOG("%s: ", __FUNCTION__);	\
				_LOG(__VA_ARGS__)
#else
#define module(name)
#define dbg(...)
#endif

#endif //_LOG_H
