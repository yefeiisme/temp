#include "stdafx.h"
#include "CenterServerConfig.h"

CCenterServerConfig	&g_pCenterServerConfig = CCenterServerConfig::Singleton();

CCenterServerConfig::CCenterServerConfig()
{
	m_pIniFile				= nullptr;

	m_nAppPort				= 0;
	m_nAppCount				= 0;
	m_nAppRecvBuffLen		= 0;
	m_nAppSendBuffLen		= 0;
	m_nAppMaxRecvPackLen	= 0;
	m_nAppMaxSendPackLen	= 0;
	m_nAppSleepTime			= 0;
	m_nAppTimeOut			= 0;

	m_nWebPort				= 0;
	m_nWebCount				= 0;
	m_nWebRecvBuffLen		= 0;
	m_nWebSendBuffLen		= 0;
	m_nWebMaxRecvPackLen	= 0;
	m_nWebMaxSendPackLen	= 0;
	m_nWebSleepTime			= 0;
	m_nWebTimeOut			= 0;
}

CCenterServerConfig::~CCenterServerConfig()
{
	if (m_pIniFile)
	{
		m_pIniFile->Release();
		m_pIniFile	= nullptr;
	}
}

CCenterServerConfig &CCenterServerConfig::Singleton()
{
	static CCenterServerConfig singleton;

	return singleton;
}

bool CCenterServerConfig::Initialize()
{
	if (!LoadConfig())
		return false;

	return true;
}

bool CCenterServerConfig::LoadConfig()
{
	m_pIniFile	= OpenIniFile("server.ini");
	if (nullptr == m_pIniFile)
	{
		g_pFileLog->WriteLog("[%s][%d] Open server.ini Failed\n", __FILE__, __LINE__);
		return false;
	}

	m_pIniFile->GetInteger("CenterServer", "AppPort", 0, &m_nAppPort);
	m_pIniFile->GetInteger("CenterServer", "AppCount", 0, &m_nAppCount);
	m_pIniFile->GetInteger("CenterServer", "AppRecvBuffLen", 0, &m_nAppRecvBuffLen);
	m_pIniFile->GetInteger("CenterServer", "AppSendBuffLen", 0, &m_nAppSendBuffLen);
	m_pIniFile->GetInteger("CenterServer", "AppMaxRecvPackLen", 0, &m_nAppMaxRecvPackLen);
	m_pIniFile->GetInteger("CenterServer", "AppMaxSendPackLen", 0, &m_nAppMaxSendPackLen);
	m_pIniFile->GetInteger("CenterServer", "AppSleepTime", 0, &m_nAppSleepTime);
	m_pIniFile->GetInteger("CenterServer", "AppTimeOut", 0, &m_nAppTimeOut);

	m_pIniFile->GetInteger("CenterServer", "WebPort", 0, &m_nWebPort);
	m_pIniFile->GetInteger("CenterServer", "WebCount", 0, &m_nWebCount);
	m_pIniFile->GetInteger("CenterServer", "WebRecvBuffLen", 0, &m_nWebRecvBuffLen);
	m_pIniFile->GetInteger("CenterServer", "WebSendBuffLen", 0, &m_nWebSendBuffLen);
	m_pIniFile->GetInteger("CenterServer", "WebMaxRecvPackLen", 0, &m_nWebMaxRecvPackLen);
	m_pIniFile->GetInteger("CenterServer", "WebMaxSendPackLen", 0, &m_nWebMaxSendPackLen);
	m_pIniFile->GetInteger("CenterServer", "WebSleepTime", 0, &m_nWebSleepTime);
	m_pIniFile->GetInteger("CenterServer", "WebTimeOut", 0, &m_nWebTimeOut);

	m_pIniFile->Release();
	m_pIniFile	= nullptr;

	return true;
}
