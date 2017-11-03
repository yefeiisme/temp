#include "stdafx.h"
#include "DataServerConfig.h"

CDataServerConfig	&g_pDataServerConfig = CDataServerConfig::Singleton();

CDataServerConfig::CDataServerConfig()
{
	m_pIniFile				= nullptr;

	m_nDataPort				= 0;
	m_nDataCount			= 0;
	m_nDataRecvBuffLen		= 0;
	m_nDataSendBuffLen		= 0;
	m_nDataMaxRecvPackLen	= 0;
	m_nDataMaxSendPackLen	= 0;
	m_nDataSleepTime		= 0;
	m_nDataTimeOut			= 0;
}

CDataServerConfig::~CDataServerConfig()
{
	if (m_pIniFile)
	{
		m_pIniFile->Release();
		m_pIniFile	= nullptr;
	}
}

CDataServerConfig &CDataServerConfig::Singleton()
{
	static CDataServerConfig singleton;

	return singleton;
}

bool CDataServerConfig::Initialize()
{
	if (!LoadConfig())
		return false;

	return true;
}

bool CDataServerConfig::LoadConfig()
{
	m_pIniFile	= OpenIniFile("server.ini");
	if (nullptr == m_pIniFile)
	{
		g_pFileLog->WriteLog("[%s][%d] Open server.ini Failed\n", __FILE__, __LINE__);
		return false;
	}

	m_pIniFile->GetInteger("DataServer", "Port", 0, &m_nDataPort);
	m_pIniFile->GetInteger("DataServer", "Count", 0, &m_nDataCount);
	m_pIniFile->GetInteger("DataServer", "RecvBuffLen", 0, &m_nDataRecvBuffLen);
	m_pIniFile->GetInteger("DataServer", "SendBuffLen", 0, &m_nDataSendBuffLen);
	m_pIniFile->GetInteger("DataServer", "MaxRecvPackLen", 0, &m_nDataMaxRecvPackLen);
	m_pIniFile->GetInteger("DataServer", "MaxSendPackLen", 0, &m_nDataMaxSendPackLen);
	m_pIniFile->GetInteger("DataServer", "SleepTime", 0, &m_nDataSleepTime);
	m_pIniFile->GetInteger("DataServer", "TimeOut", 0, &m_nDataTimeOut);

	m_pIniFile->Release();
	m_pIniFile	= nullptr;

	return true;
}
