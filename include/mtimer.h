#ifndef _MTIMER_H
#define _MTIMER_H

#include <utils.h>

#if USE(TIMER_CHRONO)
#include <chrono>
#elif USE(TIMER_REAL_TIME)
#include <getRealTime.h>
#else
#error "Can not implement CTimer, define USE_TIMER_CHRONO or USE_TIMER_REAL_TIME"
#endif

namespace utils
{

enum TimerState
{
	STOPPED = 0,
	RUNNING
};

#if USE(TIMER_CHRONO)
typedef std::chrono::high_resolution_clock clock_t;
typedef std::chrono::time_point<clock_t> time_t;
#elif USE(TIMER_REAL_TIME)
typedef double time_t;
#endif

class CTimer
{
public:
	CTimer();
	virtual ~CTimer();
	virtual bool start();
	virtual bool stop();
	virtual bool isRunning();
	virtual void reset();
	virtual double getSeconds();
	virtual double getTotalSeconds();
protected:
	time_t m_start;
	time_t m_stop;
	TimerState m_state;
	double m_total;
};

class ITimer
{
public:
	ITimer();
	virtual ~ITimer();
	virtual CTimer & getTimer();
protected:
	CTimer m_timer;
};

}

#endif //_MTIMER_H
