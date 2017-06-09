#include "stdafx.h"
#include "GameServerLogic.h"
#include "Player/Player.h"

CGameServerLogic	&g_pGameServerLogic	= CGameServerLogic::Singleton();
IGameServerLogic	&g_IGameServerLogic	= g_pGameServerLogic;

CGameServerLogic::CGameServerLogic()
{
	m_uPlayerCount	= 0;

	m_mapOnlinePlayer.clear();
	m_listFreePlayer.clear();
}

CGameServerLogic::~CGameServerLogic()
{
	m_mapOnlinePlayer.clear();

	for (list<CPlayer*>::iterator Iter = m_listFreePlayer.begin(); Iter != m_listFreePlayer.end(); ++Iter)
	{
		SAFE_DELETE(*Iter);
	}
	m_listFreePlayer.clear();
}

CGameServerLogic &CGameServerLogic::Singleton()
{
	static CGameServerLogic singleton;

	return singleton;
}


bool CGameServerLogic::Initialize()
{
	m_uPlayerCount	= 3000;
	CPlayer	*pPlayerList	= new CPlayer[m_uPlayerCount];
	if (nullptr == pPlayerList)
		return false;

	for (int nIndex = 0; nIndex < m_uPlayerCount; ++nIndex)
	{
		m_listFreePlayer.push_back(&pPlayerList[nIndex]);
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
