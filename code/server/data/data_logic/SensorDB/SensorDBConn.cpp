#include "stdafx.h"
#include "SensorDBConn.h"
#include "IDataServer.h"
#include "IMysqlQuery.h"
#include "IFileLog.h"
#include "../Client/DataClient.h"
#include "DataLogic.h"

CSensorDBConn::CSensorDBConn()
{
	for (auto nIndex = 0; nIndex < SENSOR_DB_OPT_MAX; ++nIndex)
	{
		m_ProtocolFunc[nIndex]	= nullptr;
	}

	m_ProtocolFunc[SENSOR_DB_VERIFY_ACCOUNT]	= &CSensorDBConn::RecvVerifyAccount;
	m_ProtocolFunc[SENSOR_DB_ADD_SENSOR_DATA]	= &CSensorDBConn::RecvAddSensorData;

	for (auto nIndex = 0; nIndex < CLIENT_TYPE_MAX; ++nIndex)
	{
		m_pfnTypeFunc[nIndex]	= nullptr;
	}

	m_pfnTypeFunc[GLOBAL_QUERY]	= &CSensorDBConn::GlobalQuery;
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
	IMysqlResultSet	*pResultSet = g_IDataServer.GetQueryResult();
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

void CSensorDBConn::GlobalQuery(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData)
{
	(this->*m_ProtocolFunc[pCallbackData->byOpt])(pResultSet, pCallbackData);
}

void CSensorDBConn::DataQuery(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData)
{
	CDataClient	*pClient	= g_pDataLogic.GetDataClient(pCallbackData->uClientIndex, pCallbackData->uClientID);
	if (nullptr == pClient)
		return;

	pClient->ProcessDBPack(pResultSet, pCallbackData);
}

void CSensorDBConn::RecvVerifyAccount(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData)
{
}

void CSensorDBConn::RecvAddSensorData(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData)
{
}
