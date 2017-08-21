#include "stdafx.h"
#include "WebClient.h"
#include "ICenterServer.h"
#include "IMysqlQuery.h"
#include "../SensorDB/SensorDBOperation.h"

CWebClient::pfnProtocolFunc CWebClient::m_ProtocolFunc[WEB_SERVER_NET_Protocol::WEB2S::web2s_max] =
{
	&CWebClient::RecvLogin,
	&CWebClient::RecvRequestSlopeList,
	&CWebClient::RecvRequestSensorList,
	&CWebClient::RecvRequestSensorHistory,
	&CWebClient::DefaultProtocolFunc,

	&CWebClient::DefaultProtocolFunc,
	&CWebClient::DefaultProtocolFunc,
	&CWebClient::DefaultProtocolFunc,
	&CWebClient::DefaultProtocolFunc,
	&CWebClient::DefaultProtocolFunc,

	&CWebClient::DefaultProtocolFunc,
	&CWebClient::DefaultProtocolFunc,
	&CWebClient::DefaultProtocolFunc,
	&CWebClient::DefaultProtocolFunc,
	&CWebClient::DefaultProtocolFunc,

	&CWebClient::DefaultProtocolFunc,
	&CWebClient::DefaultProtocolFunc,
	&CWebClient::DefaultProtocolFunc,
	&CWebClient::DefaultProtocolFunc,
	&CWebClient::DefaultProtocolFunc,

	&CWebClient::DefaultProtocolFunc,
	&CWebClient::DefaultProtocolFunc,
	&CWebClient::DefaultProtocolFunc,
	&CWebClient::DefaultProtocolFunc,
	&CWebClient::DefaultProtocolFunc,
};

CWebClient::pfnDBRespondFunc CWebClient::m_pfnDBRespondFunc[SENSOR_DB_OPT_MAX]
{
	&CWebClient::DBResopndLoginResult,
	&CWebClient::DBResopndSlopeList,
	&CWebClient::DBResopndSensorList,
	&CWebClient::DBResopndSensorHistory,
};

CWebClient::CWebClient() : CClient()
{
}

CWebClient::~CWebClient()
{
}

void CWebClient::DoAction()
{
	ProcessNetPack();
}

void CWebClient::ProcessDBPack(SMysqlRespond *pRespond, SMysqlDataHead *pDataHead)
{
	if (pRespond->byOpt >= SENSOR_DB_OPT_MAX)
	{
		g_pFileLog->WriteLog("[%s][%d] DB Respond Invalid Protocol[%hhu]\n", __FILE__, __LINE__, pRespond->byOpt);
		return;
	}

	(this->*m_pfnDBRespondFunc[pRespond->byOpt])(pRespond, pDataHead);
}

void CWebClient::ProcessNetPack()
{
	const void		*pPack		= nullptr;
	unsigned int	uPackLen	= 0;
	BYTE			byProtocol	= 0;

	while (nullptr != (pPack = m_pClientConn->GetPack(uPackLen)))
	{
		byProtocol	= *((BYTE*)pPack);

		if (byProtocol >= WEB_SERVER_NET_Protocol::WEB2S::web2s_max)
		{
			g_pFileLog->WriteLog("[%s][%d] App Client[%u] Invalid Protocol[%hhu]\n", __FILE__, __LINE__, m_uUniqueID, byProtocol);
			return;
		}

		(this->*m_ProtocolFunc[byProtocol])(pPack, uPackLen);

		m_pClientConn->ResetTimeOut();
	};
}

void CWebClient::DefaultProtocolFunc(const void *pPack, const unsigned int uPackLen)
{
}

void CWebClient::RecvLogin(const void *pPack, const unsigned int uPackLen)
{
	WEB_SERVER_NET_Protocol::Web2S_Login	tagLoginInfo;
	BYTE	*pLoginInfo = (BYTE*)pPack + sizeof(BYTE);
	tagLoginInfo.ParseFromArray(pLoginInfo, uPackLen - sizeof(BYTE));

	SMysqlRequest	tagRequest	= {0};
	tagRequest.byOpt			= SENSOR_DB_VERIFY_ACCOUNT;
	tagRequest.uClientID		= m_uUniqueID;
	tagRequest.uClientIndex		= m_uIndex;
	tagRequest.byClientType		= WEB_CLIENT;

	IMysqlQuery	*pMysqlQuery	= g_ICenterServer.GetMysqlQuery();
	if (nullptr == pMysqlQuery)
		return;

	pMysqlQuery->PrepareProc("AccountLogin");
	pMysqlQuery->AddParam(tagLoginInfo.account().c_str());
	pMysqlQuery->AddParam(tagLoginInfo.password().c_str());
	pMysqlQuery->EndPrepareProc(tagRequest);

	pMysqlQuery->CallProc();
}

void CWebClient::RecvRequestSlopeList(const void *pPack, const unsigned int uPackLen)
{
	if (0 == m_uAccountID)
		return;

	WEB_SERVER_NET_Protocol::WEB2S_Request_Slope_List	tagRequestSlope;
	BYTE	*pLoginInfo = (BYTE*)pPack + sizeof(BYTE);
	tagRequestSlope.ParseFromArray(pLoginInfo, uPackLen - sizeof(BYTE));

	SMysqlRequest	tagRequest	= {0};
	tagRequest.byOpt			= SENSOR_DB_SLOPE_LIST;
	tagRequest.uClientID		= m_uUniqueID;
	tagRequest.uClientIndex		= m_uIndex;
	tagRequest.byClientType		= WEB_CLIENT;

	IMysqlQuery	*pMysqlQuery	= g_ICenterServer.GetMysqlQuery();
	if (nullptr == pMysqlQuery)
		return;

	pMysqlQuery->PrepareProc("LoadSlopeList");
	pMysqlQuery->AddParam(m_uAccountID);
	pMysqlQuery->AddParam(tagRequestSlope.server_id());
	pMysqlQuery->EndPrepareProc(tagRequest);

	pMysqlQuery->CallProc();
}

void CWebClient::RecvRequestSensorList(const void *pPack, const unsigned int uPackLen)
{
	if (0 == m_uAccountID)
		return;

	WEB_SERVER_NET_Protocol::WEB2S_Request_Sensor_List	tagRequestSensorList;
	BYTE	*pLoginInfo = (BYTE*)pPack + sizeof(BYTE);
	tagRequestSensorList.ParseFromArray(pLoginInfo, uPackLen - sizeof(BYTE));

	SMysqlRequest	tagRequest	= {0};
	tagRequest.byOpt			= SENSOR_DB_SENSOR_LIST;
	tagRequest.uClientID		= m_uUniqueID;
	tagRequest.uClientIndex		= m_uIndex;
	tagRequest.byClientType		= WEB_CLIENT;

	IMysqlQuery	*pMysqlQuery	= g_ICenterServer.GetMysqlQuery();
	if (nullptr == pMysqlQuery)
		return;

	pMysqlQuery->PrepareProc("LoadSensorList");
	pMysqlQuery->AddParam(m_uAccountID);
	pMysqlQuery->AddParam(tagRequestSensorList.slope_id());
	pMysqlQuery->EndPrepareProc(tagRequest);

	pMysqlQuery->CallProc();
}

void CWebClient::RecvRequestSensorHistory(const void *pPack, const unsigned int uPackLen)
{
	if (0 == m_uAccountID)
		return;

	WEB_SERVER_NET_Protocol::WEB2S_Request_Sensor_History	tagRequestSensorHistory;
	BYTE	*pLoginInfo = (BYTE*)pPack + sizeof(BYTE);
	tagRequestSensorHistory.ParseFromArray(pLoginInfo, uPackLen - sizeof(BYTE));

	SMysqlRequest	tagRequest	= {0};
	tagRequest.byOpt			= SENSOR_DB_SENSOR_LIST;
	tagRequest.uClientID		= m_uUniqueID;
	tagRequest.uClientIndex		= m_uIndex;
	tagRequest.byClientType		= WEB_CLIENT;

	IMysqlQuery	*pMysqlQuery	= g_ICenterServer.GetMysqlQuery();
	if (nullptr == pMysqlQuery)
		return;

	pMysqlQuery->PrepareProc("LoadSensorHistory");
	pMysqlQuery->AddParam(m_uAccountID);
	pMysqlQuery->AddParam(tagRequestSensorHistory.sensor_id());
	pMysqlQuery->AddParam(tagRequestSensorHistory.begin_time());
	pMysqlQuery->AddParam(tagRequestSensorHistory.end_time());
	pMysqlQuery->EndPrepareProc(tagRequest);

	pMysqlQuery->CallProc();
}

void CWebClient::DBResopndLoginResult(SMysqlRespond *pRespond, SMysqlDataHead *pDataHead)
{
	WEB_SERVER_NET_Protocol::S2Web_Login_Result	tagLoginResult;
	tagLoginResult.set_result(pRespond->nRetCode);

	WEB_SERVER_NET_Protocol::S2Web_Login_Result::ServerData	*pServerData	= tagLoginResult.add_server_list();
	if (nullptr == pServerData)
		return;

}

void CWebClient::DBResopndSlopeList(SMysqlRespond *pRespond, SMysqlDataHead *pDataHead)
{
}

void CWebClient::DBResopndSensorList(SMysqlRespond *pRespond, SMysqlDataHead *pDataHead)
{
}

void CWebClient::DBResopndSensorHistory(SMysqlRespond *pRespond, SMysqlDataHead *pDataHead)
{
}
