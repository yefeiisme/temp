#include "stdafx.h"
#include "AppClient.h"
#include "ICenterServer.h"
#include "IMysqlQuery.h"
#include "../SensorDB/SensorDBOperation.h"

CAppClient::pfnProtocolFunc CAppClient::m_ProtocolFunc[APP_SERVER_NET_Protocol::APP2S::app2s_max] =
{
	&CAppClient::RecvLogin,
	&CAppClient::RecvRequestSlopeList,
	&CAppClient::RecvRequestSensorList,
	&CAppClient::RecvRequestSensorHistory,
	&CAppClient::RecvRequestAllList,

	&CAppClient::DefaultProtocolFunc,
	&CAppClient::DefaultProtocolFunc,
	&CAppClient::DefaultProtocolFunc,
	&CAppClient::DefaultProtocolFunc,
	&CAppClient::DefaultProtocolFunc,

	&CAppClient::DefaultProtocolFunc,
	&CAppClient::DefaultProtocolFunc,
	&CAppClient::DefaultProtocolFunc,
	&CAppClient::DefaultProtocolFunc,
	&CAppClient::DefaultProtocolFunc,

	&CAppClient::DefaultProtocolFunc,
	&CAppClient::DefaultProtocolFunc,
	&CAppClient::DefaultProtocolFunc,
	&CAppClient::DefaultProtocolFunc,
	&CAppClient::DefaultProtocolFunc,

	&CAppClient::DefaultProtocolFunc,
	&CAppClient::DefaultProtocolFunc,
	&CAppClient::DefaultProtocolFunc,
	&CAppClient::DefaultProtocolFunc,
	&CAppClient::DefaultProtocolFunc,
};

CAppClient::pfnDBRespondFunc CAppClient::m_pfnDBRespondFunc[SENSOR_DB_OPT_MAX]
{
	&CAppClient::DBResopndLoginResult,
	&CAppClient::DBResopndSlopeList,
	&CAppClient::DBResopndSensorList,
	&CAppClient::DBResopndSensorHistory,
	&CAppClient::DBResopndAllList,
};

CAppClient::CAppClient() : CClient()
{
}

CAppClient::~CAppClient()
{
}

void CAppClient::DoAction()
{
	ProcessNetPack();
}

void CAppClient::ProcessDBPack(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData)
{
	if (pCallbackData->byOpt >= SENSOR_DB_OPT_MAX)
	{
		g_pFileLog->WriteLog("[%s][%d] DB Respond Invalid Protocol[%hhu]\n", __FILE__, __LINE__, pCallbackData->byOpt);
		return;
	}

	(this->*m_pfnDBRespondFunc[pCallbackData->byOpt])(pResultSet, pCallbackData);
}

void CAppClient::ProcessNetPack()
{
	const void		*pPack		= nullptr;
	unsigned int	uPackLen	= 0;
	BYTE			byProtocol	= 0;

	while (nullptr != (pPack = m_pClientConn->GetPack(uPackLen)))
	{
		byProtocol	= *((BYTE*)pPack);

		if (byProtocol >= APP_SERVER_NET_Protocol::APP2S::app2s_max)
		{
			g_pFileLog->WriteLog("[%s][%d] App Client[%u] Invalid Protocol[%hhu]\n", __FILE__, __LINE__, m_uUniqueID, byProtocol);
			return;
		}

		(this->*m_ProtocolFunc[byProtocol])(pPack, uPackLen);

		m_pClientConn->ResetTimeOut();
	};
}

void CAppClient::DefaultProtocolFunc(const void *pPack, const unsigned int uPackLen)
{
}

void CAppClient::RecvLogin(const void *pPack, const unsigned int uPackLen)
{
	APP_SERVER_NET_Protocol::App2S_Login	tagLoginInfo;
	BYTE	*pLoginInfo = (BYTE*)pPack + sizeof(BYTE);
	tagLoginInfo.ParseFromArray(pLoginInfo, uPackLen - sizeof(BYTE));

	SMysqlRequest	tagRequest	= {0};
	tagRequest.byOpt			= SENSOR_DB_VERIFY_ACCOUNT;
	tagRequest.uClientID		= m_uUniqueID;
	tagRequest.uClientIndex		= m_uIndex;
	tagRequest.byClientType		= APP_CLIENT;

	IMysqlQuery	*pMysqlQuery	= g_ICenterServer.GetMysqlQuery();
	if (nullptr == pMysqlQuery)
		return;

	pMysqlQuery->PrepareProc("AccountLogin");
	pMysqlQuery->AddParam(tagLoginInfo.account().c_str());
	pMysqlQuery->AddParam(tagLoginInfo.password().c_str());
	pMysqlQuery->EndPrepareProc(&tagRequest, sizeof(tagRequest));

	pMysqlQuery->CallProc();
}

void CAppClient::RecvRequestSlopeList(const void *pPack, const unsigned int uPackLen)
{
	if (0 == m_uAccountID)
		return;

	APP_SERVER_NET_Protocol::APP2S_Request_Slope_List	tagRequestSlope;
	BYTE	*pLoginInfo = (BYTE*)pPack + sizeof(BYTE);
	tagRequestSlope.ParseFromArray(pLoginInfo, uPackLen - sizeof(BYTE));

	SMysqlRequest	tagRequest	= {0};
	tagRequest.byOpt			= SENSOR_DB_SLOPE_LIST;
	tagRequest.uClientID		= m_uUniqueID;
	tagRequest.uClientIndex		= m_uIndex;
	tagRequest.byClientType		= APP_CLIENT;

	IMysqlQuery	*pMysqlQuery	= g_ICenterServer.GetMysqlQuery();
	if (nullptr == pMysqlQuery)
		return;

	pMysqlQuery->PrepareProc("LoadSlopeList");
	pMysqlQuery->AddParam(m_uAccountID);
	pMysqlQuery->AddParam(tagRequestSlope.server_id());
	pMysqlQuery->EndPrepareProc(&tagRequest, sizeof(tagRequest));

	pMysqlQuery->CallProc();
}

void CAppClient::RecvRequestSensorList(const void *pPack, const unsigned int uPackLen)
{
	if (0 == m_uAccountID)
		return;

	APP_SERVER_NET_Protocol::APP2S_Request_Sensor_List	tagRequestSensorList;
	BYTE	*pLoginInfo = (BYTE*)pPack + sizeof(BYTE);
	tagRequestSensorList.ParseFromArray(pLoginInfo, uPackLen - sizeof(BYTE));

	SMysqlRequest	tagRequest	= {0};
	tagRequest.byOpt			= SENSOR_DB_SENSOR_LIST;
	tagRequest.uClientID		= m_uUniqueID;
	tagRequest.uClientIndex		= m_uIndex;
	tagRequest.byClientType		= APP_CLIENT;

	IMysqlQuery	*pMysqlQuery	= g_ICenterServer.GetMysqlQuery();
	if (nullptr == pMysqlQuery)
		return;

	pMysqlQuery->PrepareProc("LoadSensorList");
	pMysqlQuery->AddParam(m_uAccountID);
	pMysqlQuery->AddParam(tagRequestSensorList.slope_id());
	pMysqlQuery->EndPrepareProc(&tagRequest, sizeof(tagRequest));

	pMysqlQuery->CallProc();
}

void CAppClient::RecvRequestSensorHistory(const void *pPack, const unsigned int uPackLen)
{
	if (0 == m_uAccountID)
		return;

	APP_SERVER_NET_Protocol::APP2S_Request_Sensor_History	tagRequestSensorHistory;
	BYTE	*pLoginInfo = (BYTE*)pPack + sizeof(BYTE);
	tagRequestSensorHistory.ParseFromArray(pLoginInfo, uPackLen - sizeof(BYTE));

	SMysqlRequest	tagRequest	= {0};
	tagRequest.byOpt			= SENSOR_DB_SENSOR_LIST;
	tagRequest.uClientID		= m_uUniqueID;
	tagRequest.uClientIndex		= m_uIndex;
	tagRequest.byClientType		= APP_CLIENT;

	IMysqlQuery	*pMysqlQuery	= g_ICenterServer.GetMysqlQuery();
	if (nullptr == pMysqlQuery)
		return;

	pMysqlQuery->PrepareProc("LoadSensorHistory");
	pMysqlQuery->AddParam(m_uAccountID);
	pMysqlQuery->AddParam(tagRequestSensorHistory.sensor_id());
	pMysqlQuery->AddParam(tagRequestSensorHistory.begin_time());
	pMysqlQuery->AddParam(tagRequestSensorHistory.end_time());
	pMysqlQuery->EndPrepareProc(&tagRequest, sizeof(tagRequest));

	pMysqlQuery->CallProc();
}

void CAppClient::RecvRequestAllList(const void *pPack, const unsigned int uPackLen)
{
	if (0 == m_uAccountID)
		return;

	SMysqlRequest	tagRequest	={ 0 };
	tagRequest.byOpt			= SENSOR_DB_SENSOR_LIST;
	tagRequest.uClientID		= m_uUniqueID;
	tagRequest.uClientIndex		= m_uIndex;
	tagRequest.byClientType		= APP_CLIENT;

	IMysqlQuery	*pMysqlQuery	= g_ICenterServer.GetMysqlQuery();
	if (nullptr == pMysqlQuery)
		return;

	pMysqlQuery->PrepareProc("LoadAllList");
	pMysqlQuery->AddParam(m_uAccountID);
	pMysqlQuery->EndPrepareProc(&tagRequest, sizeof(tagRequest));

	pMysqlQuery->CallProc();
}

void CAppClient::DBResopndLoginResult(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData)
{
}

void CAppClient::DBResopndSlopeList(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData)
{
}

void CAppClient::DBResopndSensorList(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData)
{
}

void CAppClient::DBResopndSensorHistory(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData)
{
}

void CAppClient::DBResopndAllList(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData)
{
}
