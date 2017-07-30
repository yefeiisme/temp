#ifndef __CENTER_SERVER_CONFIG_H_
#define __CENTER_SERVER_CONFIG_H_

#include "IIniFile.h"

class CCenterServerConfig
{
public:
	CCenterServerConfig();
	~CCenterServerConfig();

	static CCenterServerConfig	&Singleton();

	bool						Initialize();
private:
	bool						LoadConfig();
private:
	IIniFile					*m_pIniFile;
public:
	int							m_nAppPort;
	int							m_nMaxAppCount;
	int							m_nAppRecvBuffLen;
	int							m_nAppSendBuffLen;
	int							m_nMaxAppRecvPackLen;
	int							m_nMaxAppSendPackLen;
	int							m_nSleepTime;
};

extern CCenterServerConfig		&g_pCenterServerConfig;

#endif
