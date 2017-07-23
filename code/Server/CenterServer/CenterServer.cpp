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
//#include "Config/GameServerConfig.h"
//#include "ClientConnection/ClientConnection.h"

#if defined(WIN32) || defined(WIN64)
#define SUBDIRNAME_LOG				"CenterServerLogs\\"
#else
#define SUBDIRNAME_LOG				"./CenterServerLogs/"
#endif

#if defined(WIN32) || defined(WIN64)
BOOL WINAPI pfnOnQuitSignal(DWORD CtrlType)
{
	g_pCenterServer.Stop();

	return TRUE;
}
#else
void pfnOnQuitSignal(int nSignal)
{
	g_pGameServer.Stop();
}
#endif

CCenterServer	&g_pCenterServer	= CCenterServer::Singleton();
ICenterServer	&g_ICenterServer	= g_pCenterServer;
int				g_nTimeNow			= 0;

CCenterServer::CCenterServer()
{
	m_pAppNetwork		= nullptr;

	m_pAppConnList		= nullptr;

	m_uFrame			= 0;

	m_ullBeginTick		= 0;
	m_ullNextFrameTick	= 0;
	m_ullTickNow		= 0;

	m_uAppCount			= 0;

	m_bRunning			= false;
	m_bWaitExit			= false;
}

CCenterServer::~CCenterServer()
{
}

CCenterServer &CCenterServer::Singleton()
{
	static CCenterServer singleton;

	return singleton;
}

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

	//if (!g_pGameServerConfig.Initialize())
	//{
	//	g_pFileLog->WriteLog("g_pGameServerConfig.Initialize Failed\n");
	//	return false;
	//}

	//if (!g_IGameServerLogic.Initialize())
	//{
	//	g_pFileLog->WriteLog("g_IGameServerLogic.Initialize Failed\n");
	//	return false;
	//}

	//m_pClientList = new CClientConnection[g_pGameServerConfig.m_nClientCount];
	//if (nullptr == m_pClientList)
	//{
	//	g_pFileLog->WriteLog("Create CClientConnection[%u] Failed\n", g_pGameServerConfig.m_nClientCount);
	//	return false;
	//}

	//m_pClientNetwork = CreateServerNetwork(
	//	g_pGameServerConfig.m_nServerPort,
	//	this,
	//	&CGameServer::ClientConnected,
	//	g_pGameServerConfig.m_nClientCount,
	//	g_pGameServerConfig.m_nSendBuffLen,
	//	g_pGameServerConfig.m_nRecvBuffLen,
	//	g_pGameServerConfig.m_nMaxSendPackLen,
	//	g_pGameServerConfig.m_nMaxRecvPackLen,
	//	g_pGameServerConfig.m_nSleepTime
	//	);
	//if (nullptr == m_pClientNetwork)
	//{
	//	g_pFileLog->WriteLog("Create Server Network For CGameServer::m_pClientNetwork Failed\n");
	//	return false;
	//}

	g_pFileLog->WriteLog("CenterServer Start!\n");

	m_bRunning = true;
	m_bWaitExit = true;

	return true;
}

void CCenterServer::Run()
{

}
