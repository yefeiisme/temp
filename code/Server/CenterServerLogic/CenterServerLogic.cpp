#include "stdafx.h"
#include "CenterServerLogic.h"
#include "Config/CenterServerLogicConfig.h"
#include "User/AppUser.h"
#include "User/WebUser.h"
//#include "Player/Player.h"

CCenterServerLogic	&g_pCenterServerLogic	= CCenterServerLogic::Singleton();
ICenterServerLogic	&g_ICenterServerLogic	= g_pCenterServerLogic;

CCenterServerLogic::CCenterServerLogic()
{
	m_pAppUserList	= nullptr;
	m_pWebUserList	= nullptr;

	m_listFreeAppUser.clear();
	m_listFreeWebUser.clear();

	m_mapOnlineAppUser.clear();
	m_mapOnlineWebUser.clear();
}

CCenterServerLogic::~CCenterServerLogic()
{
	SAFE_DELETE_ARR(m_pAppUserList);
	SAFE_DELETE_ARR(m_pWebUserList);

	m_listFreeAppUser.clear();
	m_listFreeWebUser.clear();

	m_mapOnlineAppUser.clear();
	m_mapOnlineWebUser.clear();
}

CCenterServerLogic &CCenterServerLogic::Singleton()
{
	static CCenterServerLogic singleton;

	return singleton;
}


bool CCenterServerLogic::Initialize()
{
	if (!g_pCenterServerLogicConfig.Initialize())
	{
		g_pFileLog->WriteLog("[%s][%d] Failed\n", __FUNCTION__, __LINE__);
		return false;
	}

	m_pAppUserList	= new CAppUser[g_pCenterServerLogicConfig.m_nAppUserCount];
	if (nullptr == m_pAppUserList)
		return false;

	for (int nIndex = 0; nIndex < g_pCenterServerLogicConfig.m_nAppUserCount; ++nIndex)
	{
		m_listFreeAppUser.push_back(&m_pAppUserList[nIndex]);
	}

	m_pWebUserList	= new CWebUser[g_pCenterServerLogicConfig.m_nWebUserCount];
	if (nullptr == m_pWebUserList)
		return false;

	for (int nIndex = 0; nIndex < g_pCenterServerLogicConfig.m_nWebUserCount; ++nIndex)
	{
		m_listFreeWebUser.push_back(&m_pWebUserList[nIndex]);
	}

	return true;
}

void CCenterServerLogic::Run()
{
	//for (map<IClientConnection*, CPlayer*>::iterator Iter = m_mapOnlinePlayer.begin(); Iter != m_mapOnlinePlayer.end(); ++Iter)
	//{
	//	Iter->second->DoAction();
	//}
}

bool CCenterServerLogic::AppLogin(IClientConnection *pClientConnection)
{
	CAppUser	*pAppUser	= GetFreeAppUser();
	if (nullptr == pAppUser)
		return false;

	//pPlayer->AttachClient(pClientConnection);

	//m_mapOnlinePlayer[pClientConnection]	= pPlayer;

	return true;
}

void CCenterServerLogic::AppLogout(IClientConnection *pClientConnection)
{
	//map<IClientConnection*,CPlayer*>::iterator Iter = m_mapOnlinePlayer.find(pClientConnection);

	//if (Iter != m_mapOnlinePlayer.end())
	//{
	//	m_listFreePlayer.push_back(Iter->second);
	//	Iter->second->DetachClient();
	//	m_mapOnlinePlayer.erase(Iter);
	//}
}

bool CCenterServerLogic::WebLogin(IClientConnection *pClientConnection)
{
	return true;
}

void CCenterServerLogic::WebLogout(IClientConnection *pClientConnection)
{
}
