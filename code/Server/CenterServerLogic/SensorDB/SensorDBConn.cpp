#include "stdafx.h"
#include "SensorDBConn.h"
#include "ICenterServer.h"
#include "IMysqlQuery.h"
#include "IFileLog.h"
#include "../Client/AppClient.h"
#include "../Client/WebClient.h"
#include "../Client/DataClient.h"
#include "CenterServerLogic.h"

CSensorDBConn::CSensorDBConn()
{
	for (auto nIndex = 0; nIndex < SENSOR_DB_OPT_MAX; ++nIndex)
	{
		m_ProtocolFunc[nIndex]	= nullptr;
	}

	m_ProtocolFunc[SENSOR_DB_VERIFY_ACCOUNT]	= &CSensorDBConn::RecvVerifyAccount;
	m_ProtocolFunc[SENSOR_DB_SLOPE_LIST]		= &CSensorDBConn::RecvSlopeList;
	m_ProtocolFunc[SENSOR_DB_SENSOR_LIST]		= &CSensorDBConn::RecvSensorList;
	m_ProtocolFunc[SENSOR_DB_SENSOR_HISTORY]	= &CSensorDBConn::RecvSensorHistory;
	m_ProtocolFunc[SENSOR_DB_LOAD_ALL_LIST]		= &CSensorDBConn::RecvLoadAllList;

	for (auto nIndex = 0; nIndex < CLIENT_TYPE_MAX; ++nIndex)
	{
		m_pfnTypeFunc[nIndex]	= nullptr;
	}

	m_pfnTypeFunc[GLOBAL_QUERY]	= &CSensorDBConn::GlobalQuery;
	m_pfnTypeFunc[APP_CLIENT]	= &CSensorDBConn::AppQuery;
	m_pfnTypeFunc[WEB_CLIENT]	= &CSensorDBConn::WebQuery;
	m_pfnTypeFunc[DATA_CLIENT]	= &CSensorDBConn::DataQuery;
}

CSensorDBConn::~CSensorDBConn()
{
}

bool CSensorDBConn::Initialize()
{
	return true;
}

void CSensorDBConn::Run()
{
	IQueryResult	*pQueryResult	= g_ICenterServer.GetQueryResult();
	if (nullptr == pQueryResult)
		return;

	SMysqlRespond	&pRespond	= pQueryResult->GetRespond();
	if (pRespond.byClientType >= CLIENT_TYPE_MAX)
	{
		g_pFileLog->WriteLog("Invalid DB Respond Client Type[%hhu]\n", pRespond.byClientType);
		return;
	}

	if (pRespond.byOpt >= SENSOR_DB_OPT_MAX)
	{
		g_pFileLog->WriteLog("Invalid DB Respond Opt[%hhu]\n", pRespond.byOpt);
		return;
	}

	(this->*m_pfnTypeFunc[pRespond.byClientType])(pRespond, pQueryResult);
}

void CSensorDBConn::GlobalQuery(SMysqlRespond &pRespond, IQueryResult *pResult)
{
	(this->*m_ProtocolFunc[pRespond.byOpt])(pRespond, pResult);
}

void CSensorDBConn::AppQuery(SMysqlRespond &pRespond, IQueryResult *pResult)
{
	CAppClient	*pClient	= g_pCenterServerLogic.GetAppClient(pRespond.uClientIndex, pRespond.uClientID);
	if (nullptr == pClient)
		return;

	pClient->ProcessDBPack(pRespond, pResult);
}

void CSensorDBConn::WebQuery(SMysqlRespond &pRespond, IQueryResult *pResult)
{
	CWebClient	*pClient	= g_pCenterServerLogic.GetWebClient(pRespond.uClientIndex, pRespond.uClientID);
	if (nullptr == pClient)
		return;

	pClient->ProcessDBPack(pRespond, pResult);
}

void CSensorDBConn::DataQuery(SMysqlRespond &pRespond, IQueryResult *pResult)
{
	CDataClient	*pClient	= g_pCenterServerLogic.GetDataClient(pRespond.uClientIndex, pRespond.uClientID);
	if (nullptr == pClient)
		return;

	pClient->ProcessDBPack(pRespond, pResult);
}

void CSensorDBConn::RecvVerifyAccount(SMysqlRespond &pRespond, IQueryResult *pResult)
{
}

void CSensorDBConn::RecvSlopeList(SMysqlRespond &pRespond, IQueryResult *pResult)
{
}

void CSensorDBConn::RecvSensorList(SMysqlRespond &pRespond, IQueryResult *pResult)
{
}

void CSensorDBConn::RecvSensorHistory(SMysqlRespond &pRespond, IQueryResult *pResult)
{
}

void CSensorDBConn::RecvLoadAllList(SMysqlRespond &pRespond, IQueryResult *pResult)
{
}
