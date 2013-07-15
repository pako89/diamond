#ifndef _LOG_H
#define _LOG_H

#include <stdio.h>

#define RED	"\033[1;91m"
#define EC	"\033[0m"
#define YELLOW	"\033[1;93m"

#ifndef DEFAULT_VERBOSE
#define DEFAULT_VERBOSE 	1
#endif	

extern int logv;

#ifdef DUMP_BITSTREAM
extern FILE * fh_w;
extern FILE * fh_r;
#endif

#define _FILE			stderr
#define _LOG(...)		fprintf(_FILE, __VA_ARGS__)

#define inc_logv()		(logv++)
#define logv(v, ...)		if(logv>=(v)) _LOG(__VA_ARGS__)
#define log_fmtv(v, n, f, ...)	logv((v), "%-30s : " f "\n", n, __VA_ARGS__)
#define log_prop(n, f, ...)	log_fmtv(2, n, f, __VA_ARGS__)
#define log_timer(n, t)		log_fmtv(1, "Timer " n, "%.2f", (t).getTotalSeconds())
#define log_res(n, f, ...)	log_fmtv(0, n, f, __VA_ARGS__)
#define log_info(n, f, ...)	log_fmtv(0, n, f, __VA_ARGS__)
#define log_sep(v)		logv((v), "--------------------------------\n")
#ifdef DEBUG

#define dbg(...)		_LOG(__VA_ARGS__)
#define dbgf(...)		_LOG("%s: ", __FUNCTION__);	\
				_LOG(__VA_ARGS__)
#else
#define module(name)
#define dbg(...)
#endif

#endif //_LOG_H
