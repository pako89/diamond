#include <cl_timers.h>

utils::CTimer CCLTimers::m_copyToHost;
utils::CTimer CCLTimers::m_copyToDevice;
utils::CTimer CCLTimers::m_copyBuffer;
utils::CTimer CCLTimers::m_finish;
utils::CTimer CCLTimers::m_kernel;

utils::CTimer & CCLTimers::getCopyToDevice()
{
	return m_copyToDevice;
}

utils::CTimer & CCLTimers::getCopyToHost()
{
	return m_copyToHost;
}

utils::CTimer & CCLTimers::getCopyBuffer()
{
	return m_copyBuffer;
}

utils::CTimer & CCLTimers::getFinish()
{
	return m_finish;
}

utils::CTimer & CCLTimers::getKernel()
{
	return m_kernel;
}

void CCLTimers::Print()
{
	log_timer("Copy to host", CCLTimers::getCopyToHost());
	log_timer("Copy to device", CCLTimers::getCopyToDevice());
	log_timer("Copy buffer", CCLTimers::getCopyBuffer());
	log_timer("Kernel finish", CCLTimers::getFinish());
	log_timer("Enqueue kernel", CCLTimers::getKernel());
}

