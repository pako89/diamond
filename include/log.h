#ifndef _LOG_H
#define _LOG_H


#define _FILE			stderr
#define _LOG(fmt, args...)	fprintf(_FILE, fmt, ##args)

#ifdef DEBUG
#define dbg(fmt, args...)	_LOG(fmt, ##args)
#else
#define dbg(fmt, args...)	
#endif

#endif //_LOG_H
