#include "stdafx.h"
#include "DataLogic.h"
#include "Config/DataLogicConfig.h"
#include "Client/DataClient.h"
//#include "SensorDB/SensorDBConn.h"
#include "IDataServer.h"

CDataLogic	&g_pDataLogic	= CDataLogic::Singleton();
IDataLogic	&g_IDataLogic	= g_pDataLogic;

CDataLogic::CDataLogic()
{
	m_pSensorDBConn		= nullptr;

	m_pDataClientList	= nullptr;

	m_uDataID			= 0;

	m_listFreeDataClient.clear();

	m_mapOnlineDataClient.clear();
}

CDataLogic::~CDataLogic()
{
//	SAFE_DELETE(m_pSensorDBConn);

	SAFE_DELETE_ARR(m_pDataClientList);

	m_listFreeDataClient.clear();

	m_mapOnlineDataClient.clear();
}

CDataLogic &CDataLogic::Singleton()
{
	static CDataLogic singleton;

	return singleton;
}


bool CDataLogic::Initialize()
{
	if (!g_pDataLogicConfig.Initialize())
	{
		g_pFileLog->WriteLog("[%s][%d] CDataLogic::Initialize Failed\n", __FILE__, __LINE__);
		return false;
	}

	//m_pSensorDBConn		= new CSensorDBConn;
	//if (nullptr == m_pSensorDBConn)
	//{
	//	g_pFileLog->WriteLog("[%s][%d] Create SensorDBConn Failed\n", __FILE__, __LINE__);
	//	return false;
	//}

	m_pDataClientList = new CDataClient[g_pDataLogicConfig.m_nDataClientCount];
	if (nullptr == m_pDataClientList)
	{
		g_pFileLog->WriteLog("[%s][%d] Create CDataClient[%d] Failed\n", __FILE__, __LINE__, g_pDataLogicConfig.m_nDataClientCount);
		return false;
	}

	for (auto nIndex = 0; nIndex < g_pDataLogicConfig.m_nDataClientCount; ++nIndex)
	{
		m_pDataClientList[nIndex].SetIndex(nIndex);
		m_listFreeDataClient.push_back(&m_pDataClientList[nIndex]);
	}

	return true;
}

void CDataLogic::Run()
{
	//m_pSensorDBConn->Run();

	//for (auto Iter_App = m_mapOnlineAppClient.begin(); Iter_App != m_mapOnlineAppClient.end(); ++Iter_App)
	//{
	//	Iter_App->second->DoAction();
	//}

	//for (auto Iter_Web = m_mapOnlineWebClient.begin(); Iter_Web != m_mapOnlineWebClient.end(); ++Iter_Web)
	//{
	//	Iter_Web->second->DoAction();
	//}
}

bool CDataLogic::DataClientLogin(IClientConnection *pClientConnection)
{
	CDataClient	*pDataClient = GetFreeDataClient();
	if (nullptr == pDataClient)
		return false;

	pDataClient->Login(pClientConnection, ++m_uDataID);

	m_mapOnlineDataClient[pClientConnection] = pDataClient;

	g_pFileLog->WriteLog("Data Client[%llu] Login, time[%d]\n", m_uDataID, g_nTimeNow);

	return true;
}

void CDataLogic::DataClientLogout(IClientConnection *pClientConnection)
{
	auto Iter = m_mapOnlineDataClient.find(pClientConnection);

	if (Iter != m_mapOnlineDataClient.end())
	{
		g_pFileLog->WriteLog("Data Client[%llu] Logout, time[%d]\n", Iter->second->GetUniqueID(), g_nTimeNow);

		m_listFreeDataClient.push_back(Iter->second);
		Iter->second->Logout();
		m_mapOnlineDataClient.erase(Iter);
	}
}

CDataClient *CDataLogic::GetDataClient(const UINT uClientIndex, const uint64 uClientID)
{
	if (uClientIndex >= g_pDataLogicConfig.m_nDataClientCount)
		return nullptr;

	if (m_pDataClientList[uClientIndex].GetUniqueID() != uClientID)
		return nullptr;

	return &m_pDataClientList[uClientIndex];
}
