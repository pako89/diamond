#ifndef _MTIMER_H
#define _MTIMER_H

#include <time.h>

namespace utils
{

enum TimerState
{
	STOPPED = 0,
	RUNNING
};

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
	clock_t m_start;
	clock_t m_stop;
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
