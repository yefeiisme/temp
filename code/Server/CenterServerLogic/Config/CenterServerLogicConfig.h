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
	int								m_nAppClientCount;
	int								m_nWebClientCount;
	int								m_nDataClientCount;
};

extern CCenterServerLogicConfig		&g_pCenterServerLogicConfig;

#endif
