#include "stdafx.h"
#include "CenterServerLogicConfig.h"

CCenterServerLogicConfig	&g_pCenterServerLogicConfig	= CCenterServerLogicConfig::Singleton();

CCenterServerLogicConfig::CCenterServerLogicConfig()
{
	m_pIniFile			= nullptr;

	m_nAppUserCount		= 0;
	m_nWebUserCount		= 0;
}

CCenterServerLogicConfig::~CCenterServerLogicConfig()
{
	if (m_pIniFile)
	{
		m_pIniFile->Release();
		m_pIniFile	= nullptr;
	}
}

CCenterServerLogicConfig &CCenterServerLogicConfig::Singleton()
{
	static CCenterServerLogicConfig singleton;

	return singleton;
}

bool CCenterServerLogicConfig::Initialize()
{
	if (!LoadConfig())
		return false;

	return true;
}

bool CCenterServerLogicConfig::LoadConfig()
{
	m_pIniFile	= OpenIniFile("centerserver.ini");
	if (nullptr == m_pIniFile)
	{
		g_pFileLog->WriteLog("[%s][%d] Open centerserver.ini Failed\n", __FILE__, __LINE__);
		return false;
	}

	m_pIniFile->GetInteger("Client", "PlayerCount", 0, &m_nAppUserCount);
	m_pIniFile->GetInteger("Client", "WebUserCount", 0, &m_nWebUserCount);

	m_pIniFile->Release();
	m_pIniFile	= nullptr;

	return true;
}
