#include "stdafx.h"
#include "GameServerLogic.h"
#include "Config/GameServerLogicConfig.h"
#include "Player/Player.h"

CGameServerLogic	&g_pGameServerLogic	= CGameServerLogic::Singleton();
IGameServerLogic	&g_IGameServerLogic	= g_pGameServerLogic;

CGameServerLogic::CGameServerLogic()
{
	m_pPlayerList	= nullptr;

	m_mapOnlinePlayer.clear();
	m_listFreePlayer.clear();
}

CGameServerLogic::~CGameServerLogic()
{
	SAFE_DELETE_ARR(m_pPlayerList);

	m_mapOnlinePlayer.clear();

	m_listFreePlayer.clear();
}

CGameServerLogic &CGameServerLogic::Singleton()
{
	static CGameServerLogic singleton;

	return singleton;
}


bool CGameServerLogic::Initialize()
{
	if (!g_pGameServerLogicConfig.Initialize())
	{
		g_pFileLog->WriteLog("[%s][%d] Failed\n", __FUNCTION__, __LINE__);
		return false;
	}

	m_pPlayerList	= new CPlayer[g_pGameServerLogicConfig.m_nPlayerCount];
	if (nullptr == m_pPlayerList)
		return false;

	for (int nIndex = 0; nIndex < g_pGameServerLogicConfig.m_nPlayerCount; ++nIndex)
	{
		m_listFreePlayer.push_back(&m_pPlayerList[nIndex]);
	}

	return true;
}

void CGameServerLogic::Run()
{
	for (map<IClientConnection*, CPlayer*>::iterator Iter = m_mapOnlinePlayer.begin(); Iter != m_mapOnlinePlayer.end(); ++Iter)
	{
		Iter->second->DoAction();
	}
}

bool CGameServerLogic::ClientLogin(IClientConnection *pClientConnection)
{
	CPlayer	*pPlayer	= GetFreePlayer();
	if (nullptr == pPlayer)
		return false;

	pPlayer->AttachClient(pClientConnection);

	m_mapOnlinePlayer[pClientConnection]	= pPlayer;

	return true;
}

void CGameServerLogic::ClientLogout(IClientConnection *pClientConnection)
{
	map<IClientConnection*,CPlayer*>::iterator Iter = m_mapOnlinePlayer.find(pClientConnection);

	if (Iter != m_mapOnlinePlayer.end())
	{
		Iter->second->DetachClient();
		m_mapOnlinePlayer.erase(Iter);
	}
}

void CGameServerLogic::BroadCastAllPlayer(const void *pPack, const unsigned int uPackLen)
{
	for (map<IClientConnection*,CPlayer*>::iterator Iter = m_mapOnlinePlayer.begin(); Iter != m_mapOnlinePlayer.end(); ++Iter)
	{
		Iter->second->SendMsg(pPack, uPackLen);
	}

	g_pFileLog->WriteLog("Broad Cast Player Count[%d] Msg Len[%u]\n", m_mapOnlinePlayer.size(), uPackLen);
}
