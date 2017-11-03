#include "stdafx.h"
#include "DataLogicConfig.h"

CDataLogicConfig	&g_pDataLogicConfig	= CDataLogicConfig::Singleton();

CDataLogicConfig::CDataLogicConfig()
{
	m_pIniFile				= nullptr;

	m_nDataClientCount		= 0;
}

CDataLogicConfig::~CDataLogicConfig()
{
	if (m_pIniFile)
	{
		m_pIniFile->Release();
		m_pIniFile	= nullptr;
	}
}

CDataLogicConfig &CDataLogicConfig::Singleton()
{
	static CDataLogicConfig singleton;

	return singleton;
}

bool CDataLogicConfig::Initialize()
{
	if (!LoadConfig())
		return false;

	return true;
}

bool CDataLogicConfig::LoadConfig()
{
	m_pIniFile	= OpenIniFile("dataserver.ini");
	if (nullptr == m_pIniFile)
	{
		g_pFileLog->WriteLog("[%s][%d] Open dataserver.ini Failed\n", __FILE__, __LINE__);
		return false;
	}

	m_pIniFile->GetInteger("Client", "Count", 0, &m_nDataClientCount);

	m_pIniFile->Release();
	m_pIniFile	= nullptr;

	return true;
}
