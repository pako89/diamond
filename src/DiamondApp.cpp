#include "DiamondApp.h"

namespace diamond
{

CDiamondApp * CDiamondApp::m_instance = NULL;

CDiamondApp::CDiamondApp(void)
{
}


CDiamondApp::~CDiamondApp(void)
{
}

CDiamondApp * CDiamondApp::getInstance(void)
{
	if(NULL == m_instance)
	{
		m_instance = new CDiamondApp();
	}
	return m_instance;
}

void CDiamondApp::setName(const char * appName)
{
	if(NULL != appName)
	{
		m_appName = appName;
	}
}

const char * CDiamondApp::getName(void)
{
	return m_appName;
}

void CDiamondApp::PrintBanner(void)
{
	if(NULL != m_appName)
	{
		printf("%s\n", m_appName);
	}
}

}
