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

	for (auto nIndex = 0; nIndex < g_pCenterServerLogicConfig.m_nWebUserCount; ++nIndex)
	{
		m_listFreeWebUser.push_back(&m_pWebUserList[nIndex]);
	}

	return true;
}

void CCenterServerLogic::Run()
{
	for (auto Iter_App = m_mapOnlineAppUser.begin(); Iter_App != m_mapOnlineAppUser.end(); ++Iter_App)
	{
		Iter_App->second->DoAction();
	}

	for (auto Iter_Web = m_mapOnlineWebUser.begin(); Iter_Web != m_mapOnlineWebUser.end(); ++Iter_Web)
	{
		Iter_Web->second->DoAction();
	}
}

bool CCenterServerLogic::AppLogin(IClientConnection *pClientConnection)
{
	CAppUser	*pAppUser	= GetFreeAppUser();
	if (nullptr == pAppUser)
		return false;

	pAppUser->AttachClient(pClientConnection);

	m_mapOnlineAppUser[pClientConnection]	= pAppUser;

	return true;
}

void CCenterServerLogic::AppLogout(IClientConnection *pClientConnection)
{
	auto Iter = m_mapOnlineAppUser.find(pClientConnection);

	if (Iter != m_mapOnlineAppUser.end())
	{
		m_listFreeAppUser.push_back(Iter->second);
		Iter->second->DetachClient();
		m_mapOnlineAppUser.erase(Iter);
	}
}

bool CCenterServerLogic::WebLogin(IClientConnection *pClientConnection)
{
	CWebUser	*pWebUser	= GetFreeWebUser();
	if (nullptr == pWebUser)
		return false;

	pWebUser->AttachClient(pClientConnection);

	m_mapOnlineWebUser[pClientConnection]	= pWebUser;

	return true;
}

void CCenterServerLogic::WebLogout(IClientConnection *pClientConnection)
{
	auto Iter = m_mapOnlineWebUser.find(pClientConnection);

	if (Iter != m_mapOnlineWebUser.end())
	{
		m_listFreeWebUser.push_back(Iter->second);
		Iter->second->DetachClient();
		m_mapOnlineWebUser.erase(Iter);
	}
}
