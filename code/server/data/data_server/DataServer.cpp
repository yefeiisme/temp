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
#include "DataServer.h"
#include "Config/DataServerConfig.h"
#include "Connection/ClientConnection.h"
#include "Connection/DataConnection.h"
#include "IDataLogic.h"

#if defined(WIN32) || defined(WIN64)
#define SUBDIRNAME_LOG				"data_logs\\"
#else
#define SUBDIRNAME_LOG				"./data_logs/"
#endif

#if defined(WIN32) || defined(WIN64)
BOOL WINAPI pfnOnQuitSignal(DWORD CtrlType)
{
	g_pDataServer.Stop();

	return TRUE;
}
#else
void pfnOnQuitSignal(int nSignal)
{
	g_pDataServer.Stop();
}
#endif

CDataServer	&g_pDataServer	= CDataServer::Singleton();
IDataServer	&g_IDataServer	= g_pDataServer;
int			g_nTimeNow			= 0;

CDataServer::CDataServer()
{
	m_pSensorQuery		= nullptr;

	m_pDataNetwork		= nullptr;

	m_pDataConnList		= nullptr;

	m_uFrame			= 0;

	m_ullBeginTick		= 0;
	m_ullNextFrameTick	= 0;
	m_ullTickNow		= 0;

	m_uDataConnCount	= 0;

	m_bRunning			= false;
	m_bWaitExit			= false;
}

CDataServer::~CDataServer()
{
	if (m_pSensorQuery)
	{
		m_pSensorQuery->Release();
		m_pSensorQuery	= nullptr;
	}

	if (m_pDataNetwork)
	{
		m_pDataNetwork->Release();
		m_pDataNetwork = nullptr;
	}

	SAFE_DELETE_ARR(m_pDataConnList);
}

CDataServer &CDataServer::Singleton()
{
	static CDataServer singleton;

	return singleton;
}

bool CDataServer::Initialize(const bool bDaemon)
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

	if (!g_pDataServerConfig.Initialize())
	{
		g_pFileLog->WriteLog("g_pDataServerConfig.Initialize Failed\n");
		return false;
	}

	if (!g_IDataLogic.Initialize())
	{
		g_pFileLog->WriteLog("g_IDataLogic.Initialize Failed\n");
		return false;
	}

	m_pSensorQuery	= CreateMysqlQuery("server.ini", "SensorDB");
	if (nullptr == m_pSensorQuery)
	{
		g_pFileLog->WriteLog("Create Mysql Query For Sensor Failed\n");
		return false;
	}

	m_pDataConnList = new CDataConnection[g_pDataServerConfig.m_nDataCount];
	if (nullptr == m_pDataConnList)
	{
		g_pFileLog->WriteLog("Create CDataConnection[%d] Failed\n", g_pDataServerConfig.m_nDataCount);
		return false;
	}

	m_pDataNetwork = CreateServerNetwork(
		g_pDataServerConfig.m_nDataPort,
		this,
		&CDataServer::DataConnConnected,
		g_pDataServerConfig.m_nDataCount,
		g_pDataServerConfig.m_nDataSendBuffLen,
		g_pDataServerConfig.m_nDataRecvBuffLen,
		g_pDataServerConfig.m_nDataMaxSendPackLen,
		g_pDataServerConfig.m_nDataMaxRecvPackLen,
		g_pDataServerConfig.m_nDataSleepTime
		);
	if (nullptr == m_pDataNetwork)
	{
		g_pFileLog->WriteLog("Create Server Network For CDataServer::m_pDataNetwork Failed\n");
		return false;
	}

	g_pFileLog->WriteLog("Data Server Start!\n");

	m_bRunning = true;
	m_bWaitExit = true;

	return true;
}

void CDataServer::Run()
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

		m_ullNextFrameTick	= m_ullBeginTick + m_uFrame * 1000 / 20;

		g_nTimeNow	= time(nullptr);

		g_IDataLogic.Run();

		ProcessDataConn();
	}

	m_pSensorQuery->Stop();
	m_pDataNetwork->Stop();

	while (!m_pDataNetwork->IsExited())
	{
	}

	while (!m_pSensorQuery->IsExit())
	{
	}
}

IMysqlResultSet *CDataServer::GetQueryResult()
{
	return m_pSensorQuery->GetMysqlResultSet();
}

void CDataServer::ProcessDataConn()
{
	for (auto nIndex = 0; nIndex < g_pDataServerConfig.m_nDataCount; ++nIndex)
	{
		if (m_pDataConnList[nIndex].IsIdle())
			continue;

		m_pDataConnList[nIndex].DoAction();
	}
}

void CDataServer::DataConnConnected(void *pParam, ITcpConnection *pTcpConnection, const unsigned int uIndex)
{
	CDataServer	*pDataServer = (CDataServer*)pParam;
	pDataServer->OnDataConnConnect(pTcpConnection, uIndex);
}

void CDataServer::OnDataConnConnect(ITcpConnection *pTcpConnection, const unsigned int uIndex)
{
	if (nullptr == pTcpConnection)
		return;

	if (uIndex >= g_pDataServerConfig.m_nDataCount)
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
