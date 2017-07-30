#include "stdafx.h"
#include "CenterServerConfig.h"

CCenterServerConfig	&g_pCenterServerConfig = CCenterServerConfig::Singleton();

CCenterServerConfig::CCenterServerConfig()
{
	m_pIniFile			= nullptr;
	m_nAppPort		= 0;
	m_nMaxAppCount		= 0;
	m_nAppRecvBuffLen		= 0;
	m_nAppSendBuffLen		= 0;
	m_nMaxAppRecvPackLen	= 0;
	m_nMaxAppSendPackLen	= 0;
	m_nSleepTime		= 0;
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

	m_pIniFile->GetInteger("Client", "Port", 0, &m_nAppPort);
	m_pIniFile->GetInteger("Client", "Count", 0, &m_nMaxAppCount);
	m_pIniFile->GetInteger("Client", "RecvBuffLen", 0, &m_nAppRecvBuffLen);
	m_pIniFile->GetInteger("Client", "SendBuffLen", 0, &m_nAppSendBuffLen);
	m_pIniFile->GetInteger("Client", "MaxRecvPackLen", 0, &m_nMaxAppRecvPackLen);
	m_pIniFile->GetInteger("Client", "MaxSendPackLen", 0, &m_nMaxAppSendPackLen);
	m_pIniFile->GetInteger("Client", "SleepTime", 0, &m_nSleepTime);

	m_pIniFile->Release();
	m_pIniFile	= nullptr;

	return true;
}
