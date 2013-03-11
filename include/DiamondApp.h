#ifndef _DIAMOND_APP_H
#define _DIAMOND_APP_H

#include <stdlib.h>
#include <stdio.h>
#include <exception>

namespace diamond
{

/**
 * Singleton class DiamondApp
 */
class CDiamondApp
{
public:
	static CDiamondApp * getInstance(void);

private:
	static CDiamondApp * m_instance;

public:
	virtual ~CDiamondApp(void);
	void setName(const char * appName);
	const char * getName(void);

	void PrintBanner(void);
private:
	CDiamondApp(void);
	const char * m_appName;
};

}
#endif //_DIAMOND_APP_H

