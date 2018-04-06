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

	m_nDataPort				= 0;
	m_nDataCount			= 0;
	m_nDataRecvBuffLen		= 0;
	m_nDataSendBuffLen		= 0;
	m_nDataMaxRecvPackLen	= 0;
	m_nDataMaxSendPackLen	= 0;
	m_nDataSleepTime		= 0;
	m_nDataTimeOut			= 0;
}

CCenterServerConfig::~CCenterServerConfig()
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
CCenterServerConfig &CCenterServerConfig::Singleton()
{
	static CCenterServerConfig singleton;

	return singleton;
}

//=====================================================
// 功能：主类初始化时调用本类的初始化函数
// 返回值：true初始化成功；false初始化失败
bool CCenterServerConfig::Initialize()
{
	if (!LoadConfig())
		return false;

	return true;
}

//=====================================================
// 功能：配置文件的读取，赋值相关的配置变量
//
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

	m_pIniFile->GetInteger("CenterServer", "DataPort", 0, &m_nDataPort);
	m_pIniFile->GetInteger("CenterServer", "DataCount", 0, &m_nDataCount);
	m_pIniFile->GetInteger("CenterServer", "DataRecvBuffLen", 0, &m_nDataRecvBuffLen);
	m_pIniFile->GetInteger("CenterServer", "DataSendBuffLen", 0, &m_nDataSendBuffLen);
	m_pIniFile->GetInteger("CenterServer", "DataMaxRecvPackLen", 0, &m_nDataMaxRecvPackLen);
	m_pIniFile->GetInteger("CenterServer", "DataMaxSendPackLen", 0, &m_nDataMaxSendPackLen);
	m_pIniFile->GetInteger("CenterServer", "DataSleepTime", 0, &m_nDataSleepTime);
	m_pIniFile->GetInteger("CenterServer", "DataTimeOut", 0, &m_nDataTimeOut);

	m_pIniFile->Release();
	m_pIniFile	= nullptr;

	return true;
}
