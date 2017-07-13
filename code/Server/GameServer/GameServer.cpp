#include "stdafx.h"
#include "IGameServerLogic.h"
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
#include "IFileLog.h"
#include "GameServer.h"
#include "Config/GameServerConfig.h"
#include "ClientConnection/ClientConnection.h"

#ifdef _WIN32
#define SUBDIRNAME_LOG				"GameServerLogs\\"
#else
#define SUBDIRNAME_LOG				"./GameServerLogs/"
#endif

#ifdef WIN32
BOOL WINAPI pfnOnQuitSignal(DWORD CtrlType)
{
	g_pGameServer.Stop();

	return TRUE;
}
#else
void pfnOnQuitSignal(int nSignal)
{
	g_pGameServer.Stop();
}
#endif

CGameServer	&g_pGameServer			= CGameServer::Singleton();
IGameServer	&g_IGameServer			= g_pGameServer;
int			g_nGameServerSecond		= 0;
uint64		g_ullGameServerFrame	= 0;

CGameServer::CGameServer()
{
	m_pClientNetwork	= nullptr;

	m_pClientList		= nullptr;

	m_uFrame			= 0;

	m_ullBeginTick		= 0;
	m_ullNextFrameTick	= 0;
	m_ullTickNow		= 0;

	m_bRunning			= false;
	m_bWaitExit			= false;
}

CGameServer::~CGameServer()
{
	if (m_pClientNetwork)
	{
		m_pClientNetwork->Release();
		m_pClientNetwork	= nullptr;
	}

	SAFE_DELETE_ARR(m_pClientList);
}

CGameServer &CGameServer::Singleton()
{
	static CGameServer singleton;

	return singleton;
}

bool CGameServer::Initialize()
{
	g_SetRootPath(".\\");

	g_nGameServerSecond = time(nullptr);

	if (!g_pFileLog->Initialize(SUBDIRNAME_LOG))
		return false;

	if (!DaemonInit(pfnOnQuitSignal))
	{
		g_pFileLog->WriteLog("DaemonInit Failed!\n");
		return false;
	}

	if (!g_pGameServerConfig.Initialize())
	{
		g_pFileLog->WriteLog("g_pGameServerConfig.Initialize Failed\n");
		return false;
	}

	if (!g_IGameServerLogic.Initialize())
	{
		g_pFileLog->WriteLog("g_IGameServerLogic.Initialize Failed\n");
		return false;
	}

	m_pClientList	= new CClientConnection[g_pGameServerConfig.m_nClientCount];
	if (nullptr == m_pClientList)
	{
		g_pFileLog->WriteLog("Create CClientConnection[%u] Failed\n", g_pGameServerConfig.m_nClientCount);
		return false;
	}

	m_pClientNetwork = CreateServerNetwork(
											g_pGameServerConfig.m_nServerPort,
											this,
											&CGameServer::ClientConnected,
											g_pGameServerConfig.m_nClientCount,
											g_pGameServerConfig.m_nSendBuffLen,
											g_pGameServerConfig.m_nRecvBuffLen,
											g_pGameServerConfig.m_nMaxSendPackLen,
											g_pGameServerConfig.m_nMaxRecvPackLen,
											g_pGameServerConfig.m_nSleepTime
											);
	if (nullptr == m_pClientNetwork)
	{
		g_pFileLog->WriteLog("Create Server Network For CGameServer::m_pClientNetwork Failed\n");
		return false;
	}

	g_pFileLog->WriteLog("GameServer Start!\n");

	m_bRunning	= true;
	m_bWaitExit	= true;

	return true;
}

void CGameServer::Run()
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

		g_nGameServerSecond	= time(nullptr);

		g_IGameServerLogic.Run();

		ProcessClient();
	}

	m_pClientNetwork->Stop();

	while (!m_pClientNetwork->IsExited())
	{
	}
}

void CGameServer::ProcessClient()
{
	for (int nIndex = 0; nIndex < g_pGameServerConfig.m_nClientCount; ++nIndex)
	{
		if (m_pClientList[nIndex].IsIdle())
			continue;

		m_pClientList[nIndex].DoAction();
	}
}

void CGameServer::ClientConnected(void *pParam, ITcpConnection *pTcpConnection, const unsigned int uIndex)
{
	CGameServer	*pGameServer	= (CGameServer*)pParam;
	pGameServer->OnClientConnect(pTcpConnection, uIndex);
}

void CGameServer::OnClientConnect(ITcpConnection *pTcpConnection, const unsigned int uIndex)
{
	if (nullptr == pTcpConnection)
		return;

	if (uIndex >= g_pGameServerConfig.m_nClientCount)
	{
		pTcpConnection->ShutDown();
		return;
	}

	CClientConnection	&pNewClient	= m_pClientList[uIndex];
	if (!pNewClient.IsIdle())
	{
		pTcpConnection->ShutDown();
		return;
	}

	pNewClient.Connect(pTcpConnection);
}
