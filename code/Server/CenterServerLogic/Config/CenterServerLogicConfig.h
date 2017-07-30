#ifndef __GAME_SERVER_LOGIC_CONFIG_H_
#define __GAME_SERVER_LOGIC_CONFIG_H_

#include "IIniFile.h"

class CGameServerLogicConfig
{
public:
	CGameServerLogicConfig();
	~CGameServerLogicConfig();

	static CGameServerLogicConfig	&Singleton();

	bool							Initialize();
private:
	bool							LoadConfig();
private:
	IIniFile						*m_pIniFile;
public:
	int								m_nPlayerCount;
};

extern CGameServerLogicConfig		&g_pGameServerLogicConfig;

#endif
