#ifndef __CENTER_SERVER_LOGIC_CONFIG_H_
#define __CENTER_SERVER_LOGIC_CONFIG_H_

#include "IIniFile.h"

class CCenterServerLogicConfig
{
public:
	CCenterServerLogicConfig();
	~CCenterServerLogicConfig();

	static CCenterServerLogicConfig	&Singleton();

	bool							Initialize();
private:
	bool							LoadConfig();
private:
	IIniFile						*m_pIniFile;
public:
	int								m_nAppUserCount;
	int								m_nWebUserCount;
};

extern CCenterServerLogicConfig		&g_pCenterServerLogicConfig;

#endif
