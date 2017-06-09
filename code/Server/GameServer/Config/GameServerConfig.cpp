#include "stdafx.h"
#include "GameServerConfig.h"

CGameServerConfig	&g_pGameServerConfig	= CGameServerConfig::Singleton();

CGameServerConfig::CGameServerConfig()
{
	m_pIniFile			= nullptr;
	m_nServerPort		= 0;
	m_nClientCount		= 0;
	m_nRecvBuffLen		= 0;
	m_nSendBuffLen		= 0;
	m_nMaxRecvPackLen	= 0;
	m_nMaxSendPackLen	= 0;
}

CGameServerConfig::~CGameServerConfig()
{
	if (m_pIniFile)
	{
		m_pIniFile->Release();
		m_pIniFile	= nullptr;
	}
}

CGameServerConfig &CGameServerConfig::Singleton()
{
	static CGameServerConfig singleton;

	return singleton;
}

bool CGameServerConfig::Initialize()
{
	if (!LoadConfig())
		return false;

	return true;
}

bool CGameServerConfig::LoadConfig()
{
	m_pIniFile	= OpenIniFile("gameserver.ini");
	if (nullptr == m_pIniFile)
	{
		g_pFileLog->WriteLog("[%s][%d] Open INI File Failed\n", __FILE__, __LINE__);
		return false;
	}

	m_pIniFile->GetInteger("Client", "Port", 0, &m_nServerPort);
	m_pIniFile->GetInteger("Client", "Count", 0, &m_nClientCount);
	m_pIniFile->GetInteger("Client", "RecvBuffLen", 0, &m_nRecvBuffLen);
	m_pIniFile->GetInteger("Client", "SendBuffLen", 0, &m_nSendBuffLen);
	m_pIniFile->GetInteger("Client", "MaxRecvPackLen", 0, &m_nMaxRecvPackLen);
	m_pIniFile->GetInteger("Client", "MaxSendPackLen", 0, &m_nMaxSendPackLen);

	m_pIniFile->Release();
	m_pIniFile	= nullptr;

	return true;
}
