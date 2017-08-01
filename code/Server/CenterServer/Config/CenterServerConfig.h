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
	int							m_nAppCount;
	int							m_nAppRecvBuffLen;
	int							m_nAppSendBuffLen;
	int							m_nAppMaxRecvPackLen;
	int							m_nAppMaxSendPackLen;
	int							m_nAppSleepTime;
	int							m_nAppTimeOut;

	int							m_nWebPort;
	int							m_nWebCount;
	int							m_nWebRecvBuffLen;
	int							m_nWebSendBuffLen;
	int							m_nWebMaxRecvPackLen;
	int							m_nWebMaxSendPackLen;
	int							m_nWebSleepTime;
	int							m_nWebTimeOut;
};

extern CCenterServerConfig		&g_pCenterServerConfig;

#endif
