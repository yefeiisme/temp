#include "stdafx.h"
#include "Daemon.h"
#include <time.h>
#include <thread>
#include "IFileLog.h"
#include "IIniFile.h"
#include "SimulatorLogic.h"
#include "Config/SimulatorConfig.h"
#include "ServerConnection/ServerConnection.h"

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
	m_pClientNetwork	= nullptr;
	m_pRBRequest		= nullptr;
	m_pRBRespond		= nullptr;

	m_pServerConnList	= nullptr;

	m_uFrame			= 0;

	m_ullBeginTick		= 0;
	m_ullNextFrameTick	= 0;
	m_ullTickNow		= 0;

	m_bRunning			= false;
	m_bWaitExit			= false;
}

CSimulatorLogic::~CSimulatorLogic()
{
	if (m_pClientNetwork)
	{
		m_pClientNetwork->Release();
		m_pClientNetwork	= nullptr;
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

	SAFE_DELETE_ARR(m_pServerConnList);
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

	m_pServerConnList	= new CServerConnection[g_pSimulatorConfig.m_nConnectionCount];
	if (nullptr == m_pServerConnList)
	{
		g_pFileLog->WriteLog("Create CClientConnection[m_uClientCount] Failed\n");
		return false;
	}

	for (unsigned int uIndex = 0; uIndex < g_pSimulatorConfig.m_nConnectionCount; ++uIndex)
	{
		m_pServerConnList[uIndex].SetIndex(uIndex);
	}

	m_pClientNetwork	= CreateClientNetwork(
												g_pSimulatorConfig.m_nConnectionCount,
												g_pSimulatorConfig.m_nSendBuffLen,
												g_pSimulatorConfig.m_nRecvBuffLen,
												g_pSimulatorConfig.m_nMaxSendPackLen,
												g_pSimulatorConfig.m_nMaxRecvPackLen,
												&CSimulatorLogic::ClientConnected,
												this,
												1
												);
	if (nullptr == m_pClientNetwork)
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

void CSimulatorLogic::ShutDownConnection(const unsigned int uConnIndex)
{
	m_pClientNetwork->ShutDown(uConnIndex);
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

	m_pClientNetwork->Stop();

	while (!m_pClientNetwork->IsExited())
	{
	}
}

void CSimulatorLogic::ProcessConnection()
{
	for (int nIndex = 0; nIndex < g_pSimulatorConfig.m_nConnectionCount; ++nIndex)
	{
		if (m_pServerConnList[nIndex].IsIdle())
		{
			if (m_pClientNetwork->ConnectTo(g_pSimulatorConfig.m_strServerIP, g_pSimulatorConfig.m_nServerPort, nIndex))
			{
				m_pServerConnList[nIndex].ConnectWait();
			}

			continue;
		}

		m_pServerConnList[nIndex].DoAction();
	}
}

void CSimulatorLogic::ProcessRequest()
{
	const void		*pPack		= nullptr;
	unsigned int	uPackLen	= 0;

	while (nullptr != (pPack = m_pRBRequest->RcvPack(uPackLen)))
	{
		m_pServerConnList[0].PutPack(pPack, uPackLen);
	};
}

void CSimulatorLogic::ClientConnected(void *pParam, ITcpConnection *pTcpConnection, const unsigned int uIndex)
{
	CSimulatorLogic	*pSimulatorLogic	= (CSimulatorLogic*)pParam;
	pSimulatorLogic->OnClientConnect(pTcpConnection, uIndex);
}

void CSimulatorLogic::OnClientConnect(ITcpConnection *pTcpConnection, const unsigned int uIndex)
{
	if (nullptr == pTcpConnection)
		return;

	if (uIndex >= g_pSimulatorConfig.m_nConnectionCount)
	{
		pTcpConnection->ShutDown();
		return;
	}

	CServerConnection	&pNewClient	= m_pServerConnList[uIndex];
	pNewClient.Connect(*pTcpConnection);
}
