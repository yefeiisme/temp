#include "stdafx.h"
#include "SimulatorConfig.h"

CSimulatorConfig	&g_pSimulatorConfig	= CSimulatorConfig::Singleton();

CSimulatorConfig::CSimulatorConfig()
{
	m_pIniFile			= nullptr;
	m_nServerPort		= 0;
	m_nConnectionCount		= 0;
	m_nRecvBuffLen		= 0;
	m_nSendBuffLen		= 0;
	m_nMaxRecvPackLen	= 0;
	m_nMaxSendPackLen	= 0;

	memset(m_strServerIP, 0, sizeof(m_strServerIP));
}

CSimulatorConfig::~CSimulatorConfig()
{
	if (m_pIniFile)
	{
		m_pIniFile->Release();
		m_pIniFile	= nullptr;
	}
}

CSimulatorConfig &CSimulatorConfig::Singleton()
{
	static CSimulatorConfig singleton;

	return singleton;
}

bool CSimulatorConfig::Initialize()
{
	if (!LoadConfig())
		return false;

	return true;
}

bool CSimulatorConfig::LoadConfig()
{
	m_pIniFile	= OpenIniFile("clientsimulator.ini");
	if (nullptr == m_pIniFile)
	{
		g_pFileLog->WriteLog("[%s][%d] Open INI File Failed\n", __FILE__, __LINE__);
		return false;
	}

	m_pIniFile->GetInteger("Server", "ServerPort", 0, &m_nServerPort);
	m_pIniFile->GetInteger("Server", "ConnectionCount", 0, &m_nConnectionCount);
	m_pIniFile->GetInteger("Server", "RecvBuffLen", 0, &m_nRecvBuffLen);
	m_pIniFile->GetInteger("Server", "SendBuffLen", 0, &m_nSendBuffLen);
	m_pIniFile->GetInteger("Server", "MaxRecvPackLen", 0, &m_nMaxRecvPackLen);
	m_pIniFile->GetInteger("Server", "MaxSendPackLen", 0, &m_nMaxSendPackLen);

	m_pIniFile->GetString("Server", "ServerIP", "", m_strServerIP, sizeof(m_strServerIP));
	m_strServerIP[sizeof(m_strServerIP)-1]	= '\0';

	m_pIniFile->Release();
	m_pIniFile	= nullptr;

	return true;
}
