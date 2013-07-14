#ifndef _CL_TIMERS_H
#define _CL_TIMERS_H

#include <utils.h>
#include <mtimer.h>

class CCLTimers
{
public:
	static utils::CTimer & getCopyToDevice();
	static utils::CTimer & getCopyToHost();
	static utils::CTimer & getCopyBuffer();
	static utils::CTimer & getFinish();
	static void Print();
protected:
	static utils::CTimer m_copyToHost;
	static utils::CTimer m_copyToDevice;
	static utils::CTimer m_copyBuffer;
	static utils::CTimer m_finish;
};



#endif // _CL_TIMERS_H
