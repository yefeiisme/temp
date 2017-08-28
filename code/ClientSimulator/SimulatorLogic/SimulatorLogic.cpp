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
	SAFE_DELETE_ARR(m_pAppServerConnList);
	SAFE_DELETE_ARR(m_pWebServerConnList);

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

	m_pAppServerConnList	= new CAppServerConnection[g_pSimulatorConfig.m_nConnectionCount];
	if (nullptr == m_pAppServerConnList)
	{
		g_pFileLog->WriteLog("Create CAppServerConnection[%d] Failed\n", g_pSimulatorConfig.m_nConnectionCount);
		return false;
	}

	for (unsigned int uIndex = 0; uIndex < g_pSimulatorConfig.m_nConnectionCount; ++uIndex)
	{
		m_pAppServerConnList[uIndex].SetIndex(uIndex);
	}

	m_pAppClientNetwork	= CreateClientNetwork(
												g_pSimulatorConfig.m_nConnectionCount,
												g_pSimulatorConfig.m_nSendBuffLen,
												g_pSimulatorConfig.m_nRecvBuffLen,
												g_pSimulatorConfig.m_nMaxSendPackLen,
												g_pSimulatorConfig.m_nMaxRecvPackLen,
												&CSimulatorLogic::AppClientConnected,
												this,
												1
												);
	m_pWebServerConnList	= new CWebServerConnection[g_pSimulatorConfig.m_nConnectionCount];
	if (nullptr == m_pWebServerConnList)
	{
		g_pFileLog->WriteLog("Create CWebServerConnection[%d] Failed\n", g_pSimulatorConfig.m_nConnectionCount);
		return false;
	}

	for (unsigned int uIndex = 0; uIndex < g_pSimulatorConfig.m_nConnectionCount; ++uIndex)
	{
		m_pWebServerConnList[uIndex].SetIndex(uIndex);
	}

	m_pWebClientNetwork	= CreateClientNetwork(
												g_pSimulatorConfig.m_nConnectionCount,
												g_pSimulatorConfig.m_nSendBuffLen,
												g_pSimulatorConfig.m_nRecvBuffLen,
												g_pSimulatorConfig.m_nMaxSendPackLen,
												g_pSimulatorConfig.m_nMaxRecvPackLen,
												&CSimulatorLogic::AppClientConnected,
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

void CSimulatorLogic::ShutDownAppServerConnection(const unsigned int uConnIndex)
{
	m_pAppClientNetwork->ShutDown(uConnIndex);
}

void CSimulatorLogic::ShutDownWebServerConnection(const unsigned int uConnIndex)
{
	m_pWebClientNetwork->ShutDown(uConnIndex);
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
	for (int nIndex = 0; nIndex < g_pSimulatorConfig.m_nConnectionCount; ++nIndex)
	{
		if (m_pAppServerConnList[nIndex].IsIdle())
		{
			if (m_pAppClientNetwork->ConnectTo(g_pSimulatorConfig.m_strServerIP, g_pSimulatorConfig.m_nAppServerPort, nIndex))
			{
				m_pAppServerConnList[nIndex].ConnectWait();
			}

			continue;
		}

		m_pAppServerConnList[nIndex].DoAction();
	}

	for (int nIndex = 0; nIndex < g_pSimulatorConfig.m_nConnectionCount; ++nIndex)
	{
		if (m_pAppServerConnList[nIndex].IsIdle())
		{
			if (m_pWebClientNetwork->ConnectTo(g_pSimulatorConfig.m_strServerIP, g_pSimulatorConfig.m_nWebServerPort, nIndex))
			{
				m_pAppServerConnList[nIndex].ConnectWait();
			}

			continue;
		}

		m_pAppServerConnList[nIndex].DoAction();
	}
}

void CSimulatorLogic::ProcessRequest()
{
	const void		*pPack		= nullptr;
	unsigned int	uPackLen	= 0;

	while (nullptr != (pPack = m_pRBRequest->RcvPack(uPackLen)))
	{
		m_pAppServerConnList[0].PutPack(pPack, uPackLen);
	};
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

	if (uIndex >= g_pSimulatorConfig.m_nConnectionCount)
	{
		pTcpConnection->ShutDown();
		return;
	}

	CAppServerConnection	&pNewClient	= m_pAppServerConnList[uIndex];
	pNewClient.Connect(*pTcpConnection);
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

	if (uIndex >= g_pSimulatorConfig.m_nConnectionCount)
	{
		pTcpConnection->ShutDown();
		return;
	}

	CWebServerConnection	&pNewClient	= m_pWebServerConnList[uIndex];
	pNewClient.Connect(*pTcpConnection);
}
