#ifndef _LOG_H
#define _LOG_H


#define _FILE			stderr
#define _LOG(...)	fprintf(_FILE, __VA_ARGS__)

#ifdef DEBUG

#define module(name)	
#define dbg(...)	_LOG(__VA_ARGS__)
#define dbgf(...)	_LOG("%s: ", __FUNCTION__);	\
			_LOG(__VA_ARGS__)
#else
#define module(name)	do{}while(0)
#define dbg(...)	do{}while(0)
#endif

#endif //_LOG_H
