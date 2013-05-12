#include <mtimer.h>

namespace utils
{

CTimer::CTimer() :
	m_start(0),
	m_stop(0),
	m_state(STOPPED),
	m_total(0.0)
{}

CTimer::~CTimer()
{
}

bool CTimer::start()
{
	switch(m_state)
	{
	case STOPPED:
		m_state = RUNNING;
		m_start = clock();
		break;
	case RUNNING:
		return false;
	default:
		return false;
	}
	return true;
}

bool CTimer::stop()
{
	switch(m_state)
	{
	case STOPPED:
		return false;
	case RUNNING:
		m_state = STOPPED;
		m_stop = clock();
		m_total += getSeconds();
		break;
	default:
		return false;
	}
}

bool CTimer::isRunning()
{
	return (m_state == RUNNING);
}

double CTimer::getSeconds()
{
	return (double)(m_stop - m_start)/(double)CLOCKS_PER_SEC;
}

double CTimer::getTotalSeconds()
{
	return m_total;
}

void CTimer::reset()
{
	m_total = 0.0;
	m_start = 0;
	m_stop = 0;
	m_state = STOPPED;
}

ITimer::ITimer()
{
}

ITimer::~ITimer()
{
}

CTimer & ITimer::getTimer()
{
	return m_timer;
}

}
