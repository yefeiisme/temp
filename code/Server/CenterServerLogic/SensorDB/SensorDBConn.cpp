#include "stdafx.h"
#include "SensorDBConn.h"
#include "ICenterServer.h"
#include "IMysqlQuery.h"
#include "IFileLog.h"

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
	unsigned int	uPakcLen	= 0;
	const void		*pPack		= g_ICenterServer.GetDBRespond(uPakcLen);

	if (nullptr == pPack)
		return;

	SMysqlRespond	*pRespond	= (SMysqlRespond*)pPack;
	if (pRespond->byOpt >= SENSOR_DB_OPT_MAX)
	{
		g_pFileLog->WriteLog("Invalid DB Respond Opt[%hhu]", pRespond->byOpt);
		return;
	}

	(this->*m_ProtocolFunc[pRespond->byOpt])(pPack, uPakcLen);
}

void CSensorDBConn::RecvVerifyAccount(const void *pPack, const unsigned int uPackLen)
{
}

void CSensorDBConn::RecvSlopeList(const void *pPack, const unsigned int uPackLen)
{
}

void CSensorDBConn::RecvSensorList(const void *pPack, const unsigned int uPackLen)
{
}

void CSensorDBConn::RecvSensorHistory(const void *pPack, const unsigned int uPackLen)
{
}
