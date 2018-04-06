#include "stdafx.h"
#ifdef WIN32
#include <Windows.h>
#else
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <limits.h>
#include <dirent.h>
#endif
#include <time.h>
#include "Daemon.h"
#include "CenterServer.h"
#include "Config/CenterServerConfig.h"
#include "Connection/ClientConnection.h"
#include "Connection/AppConnection.h"
#include "Connection/WebConnection.h"
#include "Connection/DataConnection.h"
#include "ICenterServerLogic.h"

#if defined(WIN32) || defined(WIN64)
#define SUBDIRNAME_LOG				"CenterServerLogs\\"
#else
#define SUBDIRNAME_LOG				"./CenterServerLogs/"
#endif

//=====================================================
// 功能：截取退出信号后，程序的退出函数
//
#if defined(WIN32) || defined(WIN64)
BOOL WINAPI pfnOnQuitSignal(DWORD CtrlType)
{
	g_pCenterServer.Stop();

	return TRUE;
}
#else
void pfnOnQuitSignal(int nSignal)
{
	g_pCenterServer.Stop();
}
#endif

CCenterServer	&g_pCenterServer	= CCenterServer::Singleton();
ICenterServer	&g_ICenterServer	= g_pCenterServer;
int				g_nTimeNow			= 0;

CCenterServer::CCenterServer()
{
	m_pSensorQuery		= nullptr;

	m_pAppNetwork		= nullptr;
	m_pWebNetwork		= nullptr;
	m_pDataNetwork		= nullptr;

	m_pAppConnList		= nullptr;
	m_pWebConnList		= nullptr;
	m_pDataConnList		= nullptr;

	m_uFrame			= 0;

	m_ullBeginTick		= 0;
	m_ullNextFrameTick	= 0;
	m_ullTickNow		= 0;

	m_uAppConnCount		= 0;
	m_uWebConnCount		= 0;
	m_uDataConnCount	= 0;

	m_bRunning			= false;
	m_bWaitExit			= false;
}

CCenterServer::~CCenterServer()
{
	if (m_pSensorQuery)
	{
		m_pSensorQuery->Release();
		m_pSensorQuery	= nullptr;
	}

	if (m_pAppNetwork)
	{
		m_pAppNetwork->Release();
		m_pAppNetwork = nullptr;
	}

	if (m_pWebNetwork)
	{
		m_pWebNetwork->Release();
		m_pWebNetwork = nullptr;
	}

	if (m_pDataNetwork)
	{
		m_pDataNetwork->Release();
		m_pDataNetwork = nullptr;
	}

	SAFE_DELETE_ARR(m_pAppConnList);
	SAFE_DELETE_ARR(m_pWebConnList);
	SAFE_DELETE_ARR(m_pDataConnList);
}

//=====================================================
// 功能：主程序类的单例
// 返回值：类的静态对象
CCenterServer &CCenterServer::Singleton()
{
	static CCenterServer singleton;

	return singleton;
}

//=====================================================
// 功能：初始化函数
// 参数bDaemon：用于linux下后台运行（已废弃）
// 返回值：true初始化成功；false初始化失败
bool CCenterServer::Initialize(const bool bDaemon)
{
	g_SetRootPath(".\\");

	g_nTimeNow = time(nullptr);

	if (!g_pFileLog->Initialize(SUBDIRNAME_LOG))
		return false;

#if defined(WIN32) || defined(WIN64)
#else
	if (!DaemonInit(pfnOnQuitSignal))
	{
		g_pFileLog->WriteLog("DaemonInit Failed!\n");
		return false;
	}
#endif

	if (!g_pCenterServerConfig.Initialize())
	{
		g_pFileLog->WriteLog("g_pCenterServerConfig.Initialize Failed\n");
		return false;
	}

	if (!g_ICenterServerLogic.Initialize())
	{
		g_pFileLog->WriteLog("g_ICenterServerLogic.Initialize Failed\n");
		return false;
	}

	m_pSensorQuery	= CreateMysqlQuery("server.ini", "SensorDB");
	if (nullptr == m_pSensorQuery)
	{
		g_pFileLog->WriteLog("Create Mysql Query For Sensor Failed\n");
		return false;
	}

	m_pAppConnList = new CAppConnection[g_pCenterServerConfig.m_nAppCount];
	if (nullptr == m_pAppConnList)
	{
		g_pFileLog->WriteLog("Create CAppConnection[%d] Failed\n", g_pCenterServerConfig.m_nAppCount);
		return false;
	}

	m_pWebConnList = new CWebConnection[g_pCenterServerConfig.m_nWebCount];
	if (nullptr == m_pWebConnList)
	{
		g_pFileLog->WriteLog("Create CWebConnection[%d] Failed\n", g_pCenterServerConfig.m_nWebCount);
		return false;
	}

	m_pDataConnList = new CDataConnection[g_pCenterServerConfig.m_nDataCount];
	if (nullptr == m_pDataConnList)
	{
		g_pFileLog->WriteLog("Create CDataConnection[%d] Failed\n", g_pCenterServerConfig.m_nDataCount);
		return false;
	}

	m_pAppNetwork = CreateServerNetwork(
		g_pCenterServerConfig.m_nAppPort,
		this,
		&CCenterServer::AppConnConnected,
		g_pCenterServerConfig.m_nAppCount,
		g_pCenterServerConfig.m_nAppSendBuffLen,
		g_pCenterServerConfig.m_nAppRecvBuffLen,
		g_pCenterServerConfig.m_nAppMaxSendPackLen,
		g_pCenterServerConfig.m_nAppMaxRecvPackLen,
		g_pCenterServerConfig.m_nAppSleepTime
		);
	if (nullptr == m_pAppNetwork)
	{
		g_pFileLog->WriteLog("Create Server Network For CCenterServer::m_pAppNetwork Failed\n");
		return false;
	}

	m_pWebNetwork = CreateServerNetwork(
		g_pCenterServerConfig.m_nWebPort,
		this,
		&CCenterServer::WebConnConnected,
		g_pCenterServerConfig.m_nWebCount,
		g_pCenterServerConfig.m_nWebSendBuffLen,
		g_pCenterServerConfig.m_nWebRecvBuffLen,
		g_pCenterServerConfig.m_nWebMaxSendPackLen,
		g_pCenterServerConfig.m_nWebMaxRecvPackLen,
		g_pCenterServerConfig.m_nWebSleepTime
		);
	if (nullptr == m_pWebNetwork)
	{
		g_pFileLog->WriteLog("Create Server Network For CCenterServer::m_pWebNetwork Failed\n");
		return false;
	}

	m_pDataNetwork = CreateServerNetwork(
		g_pCenterServerConfig.m_nDataPort,
		this,
		&CCenterServer::DataConnConnected,
		g_pCenterServerConfig.m_nDataCount,
		g_pCenterServerConfig.m_nDataSendBuffLen,
		g_pCenterServerConfig.m_nDataRecvBuffLen,
		g_pCenterServerConfig.m_nDataMaxSendPackLen,
		g_pCenterServerConfig.m_nDataMaxRecvPackLen,
		g_pCenterServerConfig.m_nDataSleepTime
		);
	if (nullptr == m_pDataNetwork)
	{
		g_pFileLog->WriteLog("Create Server Network For CCenterServer::m_pDataNetwork Failed\n");
		return false;
	}

	g_pFileLog->WriteLog("CenterServer Start!\n");

	m_bRunning = true;
	m_bWaitExit = true;

	return true;
}

//=====================================================
// 功能：程序的主循环
//
void CCenterServer::Run()
{
	m_ullBeginTick		= GetMicroTick();
	m_ullNextFrameTick	= 0;
	m_ullTickNow		= 0;

	while (m_bRunning)
	{
		m_ullTickNow	= GetMicroTick();

		if (m_ullTickNow < m_ullNextFrameTick)
		{
			yield(1);
			continue;
		}

		++m_uFrame;

		m_ullNextFrameTick	= m_ullBeginTick + m_uFrame * 1000 / 24;

		g_nTimeNow	= time(nullptr);

		g_ICenterServerLogic.Run();

		ProcessAppConn();
		ProcessWebConn();
		ProcessDataConn();
	}

	m_pSensorQuery->Stop();
	m_pAppNetwork->Stop();
	m_pWebNetwork->Stop();
	m_pDataNetwork->Stop();

	while (!m_pAppNetwork->IsExited())
	{
	}

	while (!m_pWebNetwork->IsExited())
	{
	}

	while (!m_pDataNetwork->IsExited())
	{
	}

	while (!m_pSensorQuery->IsExit())
	{
	}
}

//=====================================================
// 功能：外部模块通过接口类，获取数据库访问对象的指针
// 返回值：数据库对象的接口类指针
IMysqlResultSet *CCenterServer::GetQueryResult()
{
	return m_pSensorQuery->GetMysqlResultSet();
}

//=====================================================
// 功能：主循环中，遍历app连接，根据其状态进行不同的功能处理
//
void CCenterServer::ProcessAppConn()
{
	for (auto nIndex = 0; nIndex < g_pCenterServerConfig.m_nAppCount; ++nIndex)
	{
		if (m_pAppConnList[nIndex].IsIdle())
			continue;

		m_pAppConnList[nIndex].DoAction();
	}
}

//=====================================================
// 功能：主循环中，遍历web连接，根据其状态进行不同的功能处理
//
void CCenterServer::ProcessWebConn()
{
	for (auto nIndex = 0; nIndex < g_pCenterServerConfig.m_nWebCount; ++nIndex)
	{
		if (m_pWebConnList[nIndex].IsIdle())
			continue;

		m_pWebConnList[nIndex].DoAction();
	}
}

//=====================================================
// 功能：主循环中，遍历传感器连接，根据其状态进行不同的功能处理
//
void CCenterServer::ProcessDataConn()
{
	for (auto nIndex = 0; nIndex < g_pCenterServerConfig.m_nDataCount; ++nIndex)
	{
		if (m_pDataConnList[nIndex].IsIdle())
			continue;

		m_pDataConnList[nIndex].DoAction();
	}
}

//=====================================================
// 功能：网络库中，当有app连接上来后的回调函数
//
void CCenterServer::AppConnConnected(void *pParam, ITcpConnection *pTcpConnection, const unsigned int uIndex)
{
	CCenterServer	*pCenterServer	= (CCenterServer*)pParam;
	pCenterServer->OnAppConnConnect(pTcpConnection, uIndex);
}

//=====================================================
// 功能：网络库中，当有app连接上来后的回调函数在实例类中的处理函数
//
void CCenterServer::OnAppConnConnect(ITcpConnection *pTcpConnection, const unsigned int uIndex)
{
	if (nullptr == pTcpConnection)
		return;

	if (uIndex >= g_pCenterServerConfig.m_nAppCount)
	{
		pTcpConnection->ShutDown();
		return;
	}

	CClientConnection	&pNewAppConn	= m_pAppConnList[uIndex];
	if (!pNewAppConn.IsIdle())
	{
		pTcpConnection->ShutDown();
		return;
	}

	pNewAppConn.Connect(pTcpConnection);
}

//=====================================================
// 功能：网络库中，当有web连接上来后的回调函数
//
void CCenterServer::WebConnConnected(void *pParam, ITcpConnection *pTcpConnection, const unsigned int uIndex)
{
	CCenterServer	*pCenterServer	= (CCenterServer*)pParam;
	pCenterServer->OnWebConnConnect(pTcpConnection, uIndex);
}

//=====================================================
// 功能：网络库中，当有web连接上来后的回调函数在实例类中的处理函数
//
void CCenterServer::OnWebConnConnect(ITcpConnection *pTcpConnection, const unsigned int uIndex)
{
	if (nullptr == pTcpConnection)
		return;

	if (uIndex >= g_pCenterServerConfig.m_nWebCount)
	{
		pTcpConnection->ShutDown();
		return;
	}

	CClientConnection	&pNewWebConn	= m_pWebConnList[uIndex];
	if (!pNewWebConn.IsIdle())
	{
		pTcpConnection->ShutDown();
		return;
	}

	pNewWebConn.Connect(pTcpConnection);
}

//=====================================================
// 功能：网络库中，当有传感器连接上来后的回调函数
//
void CCenterServer::DataConnConnected(void *pParam, ITcpConnection *pTcpConnection, const unsigned int uIndex)
{
	CCenterServer	*pCenterServer = (CCenterServer*)pParam;
	pCenterServer->OnDataConnConnect(pTcpConnection, uIndex);
}

//=====================================================
// 功能：网络库中，当有传感器连接上来后的回调函数在实例类中的处理函数
//
void CCenterServer::OnDataConnConnect(ITcpConnection *pTcpConnection, const unsigned int uIndex)
{
	if (nullptr == pTcpConnection)
		return;

	if (uIndex >= g_pCenterServerConfig.m_nDataCount)
	{
		pTcpConnection->ShutDown();
		return;
	}

	CClientConnection	&pNewDataConn = m_pDataConnList[uIndex];
	if (!pNewDataConn.IsIdle())
	{
		pTcpConnection->ShutDown();
		return;
	}

	pNewDataConn.Connect(pTcpConnection);
}
