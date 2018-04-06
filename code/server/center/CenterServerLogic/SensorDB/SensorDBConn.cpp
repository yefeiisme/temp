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

//=====================================================
// 功能：主循环调用类的此函数，用于处理数据库访问后的返回结果。
// 根据访问时填充的信息，将结果扔给对应的连接去处理。
void CSensorDBConn::Run()
{
	IMysqlResultSet	*pResultSet = g_ICenterServer.GetQueryResult();
	if (nullptr == pResultSet)
		return;

	SMysqlRequest	*pCallbackData	= (SMysqlRequest*)pResultSet->GetCallbackData();
	if (nullptr == pCallbackData)
		return;

	if (pCallbackData->byClientType >= CLIENT_TYPE_MAX)
	{
		g_pFileLog->WriteLog("Invalid DB Respond Client Type[%hhu]\n", pCallbackData->byClientType);
		return;
	}

	if (pCallbackData->byOpt >= SENSOR_DB_OPT_MAX)
	{
		g_pFileLog->WriteLog("Invalid DB Respond Opt[%hhu]\n", pCallbackData->byOpt);
		return;
	}

	(this->*m_pfnTypeFunc[pCallbackData->byClientType])(pResultSet, pCallbackData);
}

//=====================================================
// 功能：全局性的数据库访问结果
//
void CSensorDBConn::GlobalQuery(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData)
{
	(this->*m_ProtocolFunc[pCallbackData->byOpt])(pResultSet, pCallbackData);
}

//=====================================================
// 功能：app访问数据库的结果
//
void CSensorDBConn::AppQuery(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData)
{
	CAppClient	*pClient	= g_pCenterServerLogic.GetAppClient(pCallbackData->uClientIndex, pCallbackData->uClientID);
	if (nullptr == pClient)
		return;

	pClient->ProcessDBPack(pResultSet, pCallbackData);
}

//=====================================================
// 功能：web访问数据库的结果
//
void CSensorDBConn::WebQuery(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData)
{
	CWebClient	*pClient	= g_pCenterServerLogic.GetWebClient(pCallbackData->uClientIndex, pCallbackData->uClientID);
	if (nullptr == pClient)
		return;

	pClient->ProcessDBPack(pResultSet, pCallbackData);
}

//=====================================================
// 功能：传感器访问数据库的结果
//
void CSensorDBConn::DataQuery(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData)
{
	CDataClient	*pClient	= g_pCenterServerLogic.GetDataClient(pCallbackData->uClientIndex, pCallbackData->uClientID);
	if (nullptr == pClient)
		return;

	pClient->ProcessDBPack(pResultSet, pCallbackData);
}

void CSensorDBConn::RecvVerifyAccount(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData)
{
}

void CSensorDBConn::RecvSlopeList(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData)
{
}

void CSensorDBConn::RecvSensorList(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData)
{
}

void CSensorDBConn::RecvSensorHistory(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData)
{
}

void CSensorDBConn::RecvLoadAllList(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData)
{
}
