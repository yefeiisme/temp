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
	unsigned int	uPackLen	= 0;
	const void		*pPack		= g_ICenterServer.GetDBRespond(uPackLen);

	if (nullptr == pPack)
		return;

	SMysqlRespond	*pRespond	= (SMysqlRespond*)pPack;
	if (pRespond->byClientType >= CLIENT_TYPE_MAX)
	{
		g_pFileLog->WriteLog("Invalid DB Respond Client Type[%hhu]", pRespond->byClientType);
		return;
	}

	if (pRespond->byOpt >= SENSOR_DB_OPT_MAX)
	{
		g_pFileLog->WriteLog("Invalid DB Respond Opt[%hhu]", pRespond->byOpt);
		return;
	}

	SMysqlDataHead	*pDataHead	= (SMysqlDataHead*)((BYTE*)pPack+sizeof(SMysqlRespond));
	(this->*m_pfnTypeFunc[pRespond->byClientType])(pRespond, pDataHead);
}

void CSensorDBConn::GlobalQuery(SMysqlRespond *pRespond, SMysqlDataHead *pDataHead)
{
	(this->*m_ProtocolFunc[pRespond->byOpt])(pRespond, pDataHead);
}

void CSensorDBConn::AppQuery(SMysqlRespond *pRespond, SMysqlDataHead *pDataHead)
{
	CAppClient	*pClient	= g_pCenterServerLogic.GetAppClient(pRespond->uClientIndex, pRespond->uClientID);
	if (nullptr == pClient)
		return;

	pClient->ProcessDBPack(pRespond, pDataHead);
}

void CSensorDBConn::WebQuery(SMysqlRespond *pRespond, SMysqlDataHead *pDataHead)
{
	CWebClient	*pClient	= g_pCenterServerLogic.GetWebClient(pRespond->uClientIndex, pRespond->uClientID);
	if (nullptr == pClient)
		return;

	pClient->ProcessDBPack(pRespond, pDataHead);
}

void CSensorDBConn::DataQuery(SMysqlRespond *pRespond, SMysqlDataHead *pDataHead)
{
	CDataClient	*pClient	= g_pCenterServerLogic.GetDataClient(pRespond->uClientIndex, pRespond->uClientID);
	if (nullptr == pClient)
		return;

	pClient->ProcessDBPack(pRespond, pDataHead);
}

void CSensorDBConn::RecvVerifyAccount(SMysqlRespond *pRespond, SMysqlDataHead *pDataHead)
{
}

void CSensorDBConn::RecvSlopeList(SMysqlRespond *pRespond, SMysqlDataHead *pDataHead)
{
}

void CSensorDBConn::RecvSensorList(SMysqlRespond *pRespond, SMysqlDataHead *pDataHead)
{
}

void CSensorDBConn::RecvSensorHistory(SMysqlRespond *pRespond, SMysqlDataHead *pDataHead)
{
}