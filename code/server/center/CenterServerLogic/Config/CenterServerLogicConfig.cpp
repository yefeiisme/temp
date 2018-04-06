#include "stdafx.h"
#include "CenterServerLogicConfig.h"

CCenterServerLogicConfig	&g_pCenterServerLogicConfig	= CCenterServerLogicConfig::Singleton();

CCenterServerLogicConfig::CCenterServerLogicConfig()
{
	m_pIniFile				= nullptr;

	m_nAppClientCount		= 0;
	m_nWebClientCount		= 0;
	m_nDataClientCount		= 0;
}

CCenterServerLogicConfig::~CCenterServerLogicConfig()
{
	if (m_pIniFile)
	{
		m_pIniFile->Release();
		m_pIniFile	= nullptr;
	}
}

//=====================================================
// 功能：配置信息类的单例
// 返回值：类的静态对象
CCenterServerLogicConfig &CCenterServerLogicConfig::Singleton()
{
	static CCenterServerLogicConfig singleton;

	return singleton;
}

//=====================================================
// 功能：逻辑层初始化时调用本类的初始化函数
// 返回值：true初始化成功；false初始化失败
bool CCenterServerLogicConfig::Initialize()
{
	if (!LoadConfig())
		return false;

	return true;
}

//=====================================================
// 功能：配置文件的读取，赋值相关的配置变量
//
bool CCenterServerLogicConfig::LoadConfig()
{
	m_pIniFile	= OpenIniFile("centerserver.ini");
	if (nullptr == m_pIniFile)
	{
		g_pFileLog->WriteLog("[%s][%d] Open centerserver.ini Failed\n", __FILE__, __LINE__);
		return false;
	}

	m_pIniFile->GetInteger("User", "AppClientCount", 0, &m_nAppClientCount);
	m_pIniFile->GetInteger("User", "WebClientCount", 0, &m_nWebClientCount);
	m_pIniFile->GetInteger("User", "DataClientCount", 0, &m_nDataClientCount);

	m_pIniFile->Release();
	m_pIniFile	= nullptr;

	return true;
}
