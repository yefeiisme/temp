#ifndef __SIMULATOR_CONFIG_H_
#define __SIMULATOR_CONFIG_H_

#include "IIniFile.h"

class CSimulatorConfig
{
public:
	CSimulatorConfig();
	~CSimulatorConfig();

	static CSimulatorConfig		&Singleton();

	bool						Initialize();
private:
	bool						LoadConfig();
private:
	IIniFile					*m_pIniFile;
public:
	int							m_nAppServerPort;
	int							m_nWebServerPort;

	int							m_nRecvBuffLen;
	int							m_nSendBuffLen;
	int							m_nMaxRecvPackLen;
	int							m_nMaxSendPackLen;

	char						m_strServerIP[16];
};

extern CSimulatorConfig			&g_pSimulatorConfig;

#endif
