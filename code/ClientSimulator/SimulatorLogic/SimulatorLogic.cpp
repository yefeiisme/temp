#include "stdafx.h"
#include "Daemon.h"
#include <time.h>
#include <thread>
#include "IFileLog.h"
#include "IIniFile.h"
#include "SimulatorLogic.h"
#include "Config/SimulatorConfig.h"
#include "ServerConnection/AppServerConnection.h"
#include "ServerConnection/WebServerConnection.h"

#ifdef _WIN32
#define SUBDIRNAME_LOG				"SimulatorLogs\\"
#else
#define SUBDIRNAME_LOG				"./SimulatorLogs/"
#endif

CSimulatorLogic	&g_pSimulatorLogic	= CSimulatorLogic::Singleton();
ISimulatorLogic	&g_ISimulatorLogic	= g_pSimulatorLogic;
time_t			g_nSimulatorSecond	= time(nullptr);

CSimulatorLogic::CSimulatorLogic()
{
	memset(m_ProtocolFunc, 0, sizeof(m_ProtocolFunc));

	m_ProtocolFunc[u2l_app_login]			= &CSimulatorLogic::RecvAppLogin;
	m_ProtocolFunc[u2l_web_login]			= &CSimulatorLogic::RecvWebLogin;
	m_ProtocolFunc[u2l_app_slope_list]		= &CSimulatorLogic::RecvAppSlopeList;
	m_ProtocolFunc[u2l_web_slope_list]		= &CSimulatorLogic::RecvWebSlopeList;
	m_ProtocolFunc[u2l_app_sensor_list]		= &CSimulatorLogic::RecvAppSensorList;
	m_ProtocolFunc[u2l_web_sensor_list]		= &CSimulatorLogic::RecvWebSensorList;
	m_ProtocolFunc[u2l_app_sensor_history]	= &CSimulatorLogic::RecvAppSensorHistory;
	m_ProtocolFunc[u2l_web_sensor_history]	= &CSimulatorLogic::RecvWebSensorHistory;

	m_pAppClientNetwork		= nullptr;
	m_pWebClientNetwork		= nullptr;

	m_pRBRequest			= nullptr;
	m_pRBRespond			= nullptr;

	m_pAppServerConnList	= nullptr;
	m_pWebServerConnList	= nullptr;

	m_uFrame				= 0;

	m_ullBeginTick			= 0;
	m_ullNextFrameTick		= 0;
	m_ullTickNow			= 0;

	m_bRunning				= false;
	m_bWaitExit				= false;
}

CSimulatorLogic::~CSimulatorLogic()
{
	SAFE_DELETE(m_pAppServerConnList);
	SAFE_DELETE(m_pWebServerConnList);

	if (m_pAppClientNetwork)
	{
		m_pAppClientNetwork->Release();
		m_pAppClientNetwork	= nullptr;
	}

	if (m_pWebClientNetwork)
	{
		m_pWebClientNetwork->Release();
		m_pWebClientNetwork	= nullptr;
	}

	if (m_pRBRequest)
	{
		m_pRBRequest->Release();
		m_pRBRequest	= nullptr;
	}

	if (m_pRBRespond)
	{
		m_pRBRespond->Release();
		m_pRBRespond	= nullptr;
	}
}

CSimulatorLogic &CSimulatorLogic::Singleton()
{
	static CSimulatorLogic singleton;

	return singleton;
}

bool CSimulatorLogic::Initialize()
{
	if (m_bRunning)
	{
		g_pFileLog->WriteLog("[%s][%d] Simulator Already Running\n", __FILE__, __LINE__);
		return false;
	}

	g_SetRootPath(".\\");

	g_nSimulatorSecond = time(nullptr);

	if (!g_pFileLog->Initialize(SUBDIRNAME_LOG))
		return false;

	if (!g_pSimulatorConfig.Initialize())
		return false;

	m_pAppServerConnList	= new CAppServerConnection;
	if (nullptr == m_pAppServerConnList)
	{
		g_pFileLog->WriteLog("Create CAppServerConnection Failed\n");
		return false;
	}

	m_pAppClientNetwork	= CreateClientNetwork(
												g_pSimulatorConfig.m_nSendBuffLen,
												g_pSimulatorConfig.m_nRecvBuffLen,
												g_pSimulatorConfig.m_nMaxSendPackLen,
												g_pSimulatorConfig.m_nMaxRecvPackLen,
												&CSimulatorLogic::AppClientConnected,
												this,
												1
												);

	m_pWebServerConnList	= new CWebServerConnection;
	if (nullptr == m_pWebServerConnList)
	{
		g_pFileLog->WriteLog("Create CWebServerConnection Failed\n");
		return false;
	}

	m_pWebClientNetwork	= CreateClientNetwork(
												g_pSimulatorConfig.m_nSendBuffLen,
												g_pSimulatorConfig.m_nRecvBuffLen,
												g_pSimulatorConfig.m_nMaxSendPackLen,
												g_pSimulatorConfig.m_nMaxRecvPackLen,
												&CSimulatorLogic::WebServerConnected,
												this,
												1
												);
	if (nullptr == m_pWebClientNetwork)
	{
		g_pFileLog->WriteLog("[%s][%d] CreateClientNetwork Failed\n", __FILE__, __LINE__);
		return false;
	}

	m_pRBRequest	= CreateRingBuffer(16*1024, 2048);
	if (nullptr == m_pRBRequest)
	{
		g_pFileLog->WriteLog("[%s][%d] CreateRingBuffer(16*1024, 2048) Failed\n", __FILE__, __LINE__);
		return false;
	}

	m_pRBRespond	= CreateRingBuffer(128*1024, 2048);
	if (nullptr == m_pRBRespond)
	{
		g_pFileLog->WriteLog("[%s][%d] CreateRingBuffer(16*1024, 2048) Failed\n", __FILE__, __LINE__);
		return false;
	}

	m_bRunning	= true;
	m_bWaitExit	= true;

	std::thread	threadNetwork(&CSimulatorLogic::ThreadFunc, this);
	threadNetwork.detach();

	return true;
}

bool CSimulatorLogic::SendRequest(const void *pPack, const unsigned int uPackLen)
{
	return m_pRBRequest->SndPack(pPack, uPackLen);
}

const void *CSimulatorLogic::GetRespond(unsigned int &uPackLen)
{
	return m_pRBRespond->RcvPack(uPackLen);
}

void CSimulatorLogic::ShutDownAppServerConnection()
{
	m_pAppClientNetwork->ShutDown();
}

void CSimulatorLogic::ShutDownWebServerConnection()
{
	m_pWebClientNetwork->ShutDown();
}

void CSimulatorLogic::ThreadFunc()
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

		g_nSimulatorSecond	= time(nullptr);

		ProcessConnection();

		ProcessRequest();
	}

	m_pAppClientNetwork->Stop();

	m_pWebClientNetwork->Stop();

	while (!m_pAppClientNetwork->IsExited())
	{
	}

	while (!m_pWebClientNetwork->IsExited())
	{
	}
}

void CSimulatorLogic::ProcessConnection()
{
	if (m_pAppServerConnList->IsIdle())
	{
		m_pAppClientNetwork->ConnectTo(g_pSimulatorConfig.m_strServerIP, g_pSimulatorConfig.m_nAppServerPort);
	}

	if (m_pWebServerConnList->IsIdle())
	{
		m_pWebClientNetwork->ConnectTo(g_pSimulatorConfig.m_strServerIP, g_pSimulatorConfig.m_nWebServerPort);
	}

	m_pAppServerConnList->DoAction();
	m_pWebServerConnList->DoAction();
}

void CSimulatorLogic::AppClientConnected(void *pParam, ITcpConnection *pTcpConnection, const unsigned int uIndex)
{
	CSimulatorLogic	*pSimulatorLogic	= (CSimulatorLogic*)pParam;
	pSimulatorLogic->OnAppClientConnect(pTcpConnection, uIndex);
}

void CSimulatorLogic::OnAppClientConnect(ITcpConnection *pTcpConnection, const unsigned int uIndex)
{
	if (nullptr == pTcpConnection)
		return;

	m_pAppServerConnList->Connect(*pTcpConnection);
}

void CSimulatorLogic::WebServerConnected(void *pParam, ITcpConnection *pTcpConnection, const unsigned int uIndex)
{
	CSimulatorLogic	*pSimulatorLogic	= (CSimulatorLogic*)pParam;
	pSimulatorLogic->OnWebServerConnected(pTcpConnection, uIndex);
}

void CSimulatorLogic::OnWebServerConnected(ITcpConnection *pTcpConnection, const unsigned int uIndex)
{
	if (nullptr == pTcpConnection)
		return;

	m_pWebServerConnList->Connect(*pTcpConnection);
}
