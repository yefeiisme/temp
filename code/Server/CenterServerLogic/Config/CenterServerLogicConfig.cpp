#include "stdafx.h"
#include "GameServerLogicConfig.h"

CGameServerLogicConfig	&g_pGameServerLogicConfig	= CGameServerLogicConfig::Singleton();

CGameServerLogicConfig::CGameServerLogicConfig()
{
	m_pIniFile			= nullptr;

	m_nPlayerCount		= 0;
}

CGameServerLogicConfig::~CGameServerLogicConfig()
{
	if (m_pIniFile)
	{
		m_pIniFile->Release();
		m_pIniFile	= nullptr;
	}
}

CGameServerLogicConfig &CGameServerLogicConfig::Singleton()
{
	static CGameServerLogicConfig singleton;

	return singleton;
}

bool CGameServerLogicConfig::Initialize()
{
	if (!LoadConfig())
		return false;

	return true;
}

bool CGameServerLogicConfig::LoadConfig()
{
	m_pIniFile	= OpenIniFile("gameserverlogic.ini");
	if (nullptr == m_pIniFile)
	{
		g_pFileLog->WriteLog("[%s][%d] Open INI File Failed\n", __FILE__, __LINE__);
		return false;
	}

	m_pIniFile->GetInteger("Player", "PlayerCount", 0, &m_nPlayerCount);

	m_pIniFile->Release();
	m_pIniFile	= nullptr;

	return true;
}
