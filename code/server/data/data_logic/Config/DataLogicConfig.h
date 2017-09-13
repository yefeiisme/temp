#ifndef __CENTER_SERVER_LOGIC_CONFIG_H_
#define __CENTER_SERVER_LOGIC_CONFIG_H_

#include "IIniFile.h"

class CDataLogicConfig
{
public:
	CDataLogicConfig();
	~CDataLogicConfig();

	static CDataLogicConfig	&Singleton();

	bool					Initialize();
private:
	bool					LoadConfig();
private:
	IIniFile				*m_pIniFile;
public:
	int						m_nDataClientCount;
};

extern CDataLogicConfig		&g_pDataLogicConfig;

#endif
