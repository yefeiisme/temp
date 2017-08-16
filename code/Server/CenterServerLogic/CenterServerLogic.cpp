#include "stdafx.h"
#include "CenterServerLogic.h"
#include "Config/CenterServerLogicConfig.h"
#include "Client/AppClient.h"
#include "Client/WebClient.h"
#include "Client/DataClient.h"
#include "SensorDB/SensorDBConn.h"
#include "ICenterServer.h"

CCenterServerLogic	&g_pCenterServerLogic	= CCenterServerLogic::Singleton();
ICenterServerLogic	&g_ICenterServerLogic	= g_pCenterServerLogic;

CCenterServerLogic::CCenterServerLogic()
{
	m_pSensorDBConn		= nullptr;

	m_pAppClientList	= nullptr;
	m_pWebClientList	= nullptr;
	m_pDataClientList	= nullptr;

	m_listFreeAppClient.clear();
	m_listFreeWebClient.clear();
	m_listFreeDataClient.clear();

	m_mapOnlineAppClient.clear();
	m_mapOnlineWebClient.clear();
	m_mapOnlineDataClient.clear();
}

CCenterServerLogic::~CCenterServerLogic()
{
	SAFE_DELETE(m_pSensorDBConn);

	SAFE_DELETE_ARR(m_pAppClientList);
	SAFE_DELETE_ARR(m_pWebClientList);
	SAFE_DELETE_ARR(m_pDataClientList);

	m_listFreeAppClient.clear();
	m_listFreeWebClient.clear();
	m_listFreeDataClient.clear();

	m_mapOnlineAppClient.clear();
	m_mapOnlineWebClient.clear();
	m_mapOnlineDataClient.clear();
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
		g_pFileLog->WriteLog("[%s][%d] CCenterServerLogic::Initialize Failed\n", __FUNCTION__, __LINE__);
		return false;
	}

	m_pSensorDBConn		= new CSensorDBConn;
	if (nullptr == m_pSensorDBConn)
	{
		g_pFileLog->WriteLog("[%s][%d] Create SensorDBConn Failed\n", __FUNCTION__, __LINE__);
		return false;
	}

	m_pAppClientList	= new CAppClient[g_pCenterServerLogicConfig.m_nAppClientCount];
	if (nullptr == m_pAppClientList)
	{
		g_pFileLog->WriteLog("[%s][%d] Create CAppClient[%d] Failed\n", __FUNCTION__, __LINE__, g_pCenterServerLogicConfig.m_nAppClientCount);
		return false;
	}

	for (auto nIndex = 0; nIndex < g_pCenterServerLogicConfig.m_nAppClientCount; ++nIndex)
	{
		m_pAppClientList[nIndex].SetIndex(nIndex);
		m_listFreeAppClient.push_back(&m_pAppClientList[nIndex]);
	}

	m_pWebClientList	= new CWebClient[g_pCenterServerLogicConfig.m_nWebClientCount];
	if (nullptr == m_pWebClientList)
	{
		g_pFileLog->WriteLog("[%s][%d] Create CWebClient[%d] Failed\n", __FUNCTION__, __LINE__, g_pCenterServerLogicConfig.m_nWebClientCount);
		return false;
	}

	for (auto nIndex = 0; nIndex < g_pCenterServerLogicConfig.m_nWebClientCount; ++nIndex)
	{
		m_pWebClientList[nIndex].SetIndex(nIndex);
		m_listFreeWebClient.push_back(&m_pWebClientList[nIndex]);
	}

	m_pDataClientList = new CDataClient[g_pCenterServerLogicConfig.m_nDataClientCount];
	if (nullptr == m_pDataClientList)
	{
		g_pFileLog->WriteLog("[%s][%d] Create CDataClient[%d] Failed\n", __FUNCTION__, __LINE__, g_pCenterServerLogicConfig.m_nDataClientCount);
		return false;
	}

	for (auto nIndex = 0; nIndex < g_pCenterServerLogicConfig.m_nDataClientCount; ++nIndex)
	{
		m_pDataClientList[nIndex].SetIndex(nIndex);
		m_listFreeDataClient.push_back(&m_pDataClientList[nIndex]);
	}

	return true;
}

void CCenterServerLogic::Run()
{
	m_pSensorDBConn->Run();

	for (auto Iter_App = m_mapOnlineAppClient.begin(); Iter_App != m_mapOnlineAppClient.end(); ++Iter_App)
	{
		Iter_App->second->DoAction();
	}

	for (auto Iter_Web = m_mapOnlineWebClient.begin(); Iter_Web != m_mapOnlineWebClient.end(); ++Iter_Web)
	{
		Iter_Web->second->DoAction();
	}
}

bool CCenterServerLogic::AppClientLogin(IClientConnection *pClientConnection)
{
	CAppClient	*pAppClient	= GetFreeAppClient();
	if (nullptr == pAppClient)
		return false;

	pAppClient->AttachClient(pClientConnection);

	m_mapOnlineAppClient[pClientConnection]	= pAppClient;

	return true;
}

void CCenterServerLogic::AppClientLogout(IClientConnection *pClientConnection)
{
	auto Iter = m_mapOnlineAppClient.find(pClientConnection);

	if (Iter != m_mapOnlineAppClient.end())
	{
		m_listFreeAppClient.push_back(Iter->second);
		Iter->second->DetachClient();
		m_mapOnlineAppClient.erase(Iter);
	}
}

bool CCenterServerLogic::WebClientLogin(IClientConnection *pClientConnection)
{
	CWebClient	*pWebClient	= GetFreeWebClient();
	if (nullptr == pWebClient)
		return false;

	pWebClient->AttachClient(pClientConnection);

	m_mapOnlineWebClient[pClientConnection]	= pWebClient;

	return true;
}

void CCenterServerLogic::WebClientLogout(IClientConnection *pClientConnection)
{
	auto Iter = m_mapOnlineWebClient.find(pClientConnection);

	if (Iter != m_mapOnlineWebClient.end())
	{
		m_listFreeWebClient.push_back(Iter->second);
		Iter->second->DetachClient();
		m_mapOnlineWebClient.erase(Iter);
	}
}

bool CCenterServerLogic::DataClientLogin(IClientConnection *pClientConnection)
{
	CDataClient	*pDataClient = GetFreeDataClient();
	if (nullptr == pDataClient)
		return false;

	pDataClient->AttachClient(pClientConnection);

	m_mapOnlineDataClient[pClientConnection] = pDataClient;

	return true;
}

void CCenterServerLogic::DataClientLogout(IClientConnection *pClientConnection)
{
	auto Iter = m_mapOnlineDataClient.find(pClientConnection);

	if (Iter != m_mapOnlineDataClient.end())
	{
		m_listFreeDataClient.push_back(Iter->second);
		Iter->second->DetachClient();
		m_mapOnlineDataClient.erase(Iter);
	}
}
