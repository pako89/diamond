#ifndef _LOG_H
#define _LOG_H


#define _FILE			stderr
#define _LOG(...)	fprintf(_FILE, __VA_ARGS__)

#ifdef DEBUG
#define dbg(...)	_LOG(__VA_ARGS__)
#else
#define dbg(...)	
#endif

#endif //_LOG_H
