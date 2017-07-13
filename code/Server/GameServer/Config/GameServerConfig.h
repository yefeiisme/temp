#ifndef __GAME_SERVER_CONFIG_H_
#define __GAME_SERVER_CONFIG_H_

#include "IIniFile.h"

class CGameServerConfig
{
public:
	CGameServerConfig();
	~CGameServerConfig();

	static CGameServerConfig	&Singleton();

	bool						Initialize();
private:
	bool						LoadConfig();
private:
	IIniFile					*m_pIniFile;
public:
	int							m_nServerPort;
	int							m_nClientCount;
	int							m_nRecvBuffLen;
	int							m_nSendBuffLen;
	int							m_nMaxRecvPackLen;
	int							m_nMaxSendPackLen;
	int							m_nSleepTime;
};

extern CGameServerConfig		&g_pGameServerConfig;

#endif
