#ifndef __DATA_SERVER_CONFIG_H_
#define __DATA_SERVER_CONFIG_H_

#include "IIniFile.h"

class CDataServerConfig
{
public:
	CDataServerConfig();
	~CDataServerConfig();

	static CDataServerConfig	&Singleton();

	bool						Initialize();
private:
	bool						LoadConfig();
private:
	IIniFile					*m_pIniFile;
public:
	int							m_nDataPort;
	int							m_nDataCount;
	int							m_nDataRecvBuffLen;
	int							m_nDataSendBuffLen;
	int							m_nDataMaxRecvPackLen;
	int							m_nDataMaxSendPackLen;
	int							m_nDataSleepTime;
	int							m_nDataTimeOut;
};

extern CDataServerConfig		&g_pDataServerConfig;

#endif
