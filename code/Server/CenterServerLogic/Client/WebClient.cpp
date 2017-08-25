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
	&CWebClient::RecvRequestAllList,

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
	&CWebClient::DBResopndAllList,
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

void CWebClient::ProcessDBPack(SMysqlRespond &pRespond, IQueryResult *pResult)
{
	if (pRespond.byOpt >= SENSOR_DB_OPT_MAX)
	{
		g_pFileLog->WriteLog("[%s][%d] DB Respond Invalid Protocol[%hhu]\n", __FILE__, __LINE__, pRespond.byOpt);
		return;
	}

	(this->*m_pfnDBRespondFunc[pRespond.byOpt])(pRespond, pResult);
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

void CWebClient::RecvRequestAllList(const void *pPack, const unsigned int uPackLen)
{
	if (0 == m_uAccountID)
		return;

	SMysqlRequest	tagRequest	= {0};
	tagRequest.byOpt			= SENSOR_DB_SENSOR_LIST;
	tagRequest.uClientID		= m_uUniqueID;
	tagRequest.uClientIndex		= m_uIndex;
	tagRequest.byClientType		= WEB_CLIENT;

	IMysqlQuery	*pMysqlQuery	= g_ICenterServer.GetMysqlQuery();
	if (nullptr == pMysqlQuery)
		return;

	pMysqlQuery->PrepareProc("LoadSlopeList");
	pMysqlQuery->AddParam(m_uAccountID);
	// 临时数据
	// ...
	pMysqlQuery->AddParam(1);
	//pMysqlQuery->AddParam(tagRequestSlope.server_id());
	pMysqlQuery->EndPrepareProc(tagRequest);

	pMysqlQuery->CallProc();
}

void CWebClient::DBResopndLoginResult(SMysqlRespond &pRespond, IQueryResult *pResult)
{
	WEB_SERVER_NET_Protocol::S2Web_Login_Result	tagLoginResult;
	tagLoginResult.set_result(pRespond.nRetCode);

	UINT	uCol		= 0;
	UINT	uServerID	= 0;
	WORD	wServerPort	= 0;
	char	strServerIP[16];

	for (auto uRow = 0; uRow < pRespond.uRowCount; ++uRow)
	{
		WEB_SERVER_NET_Protocol::S2Web_Login_Result::ServerData	*pServerData = tagLoginResult.add_server_list();
		if (nullptr == pServerData)
			continue;

		uCol	= 0;

		pResult->GetData(uRow, uCol++, uServerID);
		pResult->GetData(uRow, uCol++, strServerIP, sizeof(strServerIP));
		pResult->GetData(uRow, uCol++, wServerPort);

		pServerData->set_id(uServerID);
		pServerData->set_ip(strServerIP);
		pServerData->set_port(wServerPort);
	}

	SendWebLoginResult(tagLoginResult);
}

void CWebClient::DBResopndSlopeList(SMysqlRespond &pRespond, IQueryResult *pResult)
{
	WEB_SERVER_NET_Protocol::S2Web_Slope_List	tagSlopeList;

	UINT	uCol			= 0;
	WORD	wSlopeID		= 0;
	BYTE	bySlopeType		= 0;
	BYTE	bySlopeState	= 0;
	double	dLongitude		= 0.0f;
	double	dLatitude		= 0.0f;
	char	strName[128];

	for (auto uRow = 0; uRow < pRespond.uRowCount; ++uRow)
	{
		WEB_SERVER_NET_Protocol::S2Web_Slope_List::SlopeData	*pSlopeData	= tagSlopeList.add_slope_list();
		if (nullptr == pSlopeData)
			continue;

		uCol	= 0;

		pResult->GetData(uRow, uCol++, wSlopeID);
		pResult->GetData(uRow, uCol++, bySlopeType);
		pResult->GetData(uRow, uCol++, strName, sizeof(strName));
		pResult->GetData(uRow, uCol++, bySlopeState);
		pResult->GetData(uRow, uCol++, dLongitude);
		pResult->GetData(uRow, uCol++, dLatitude);

		pSlopeData->set_id(wSlopeID);
		pSlopeData->set_type(bySlopeType);
		pSlopeData->set_name(strName);
		pSlopeData->set_state(bySlopeState);
		pSlopeData->set_longitude(dLongitude);
		pSlopeData->set_latitude(dLatitude);
	}

	SendWebSlopeList(tagSlopeList);
}

void CWebClient::DBResopndSensorList(SMysqlRespond &pRespond, IQueryResult *pResult)
{
	WEB_SERVER_NET_Protocol::S2Web_Sensor_List	tagSensorList;

	UINT	uCol			= 0;
	UINT	uSensorID		= 0;
	BYTE	bySensorType	= 0;
	BYTE	bySensorState	= 0;
	WORD	wSlopeID		= 0;
	double	dLongitude		= 0.0f;
	double	dLatitude		= 0.0f;
	double	dCurValue1		= 0.0f;
	double	dCurValue2		= 0.0f;
	double	dCurValue3		= 0.0f;
	double	dAvgValue1		= 0.0f;
	double	dAvgValue2		= 0.0f;
	double	dAvgValue3		= 0.0f;

	for (auto uRow = 0; uRow < pRespond.uRowCount; ++uRow)
	{
		WEB_SERVER_NET_Protocol::S2Web_Sensor_List::SensorData	*pSensorData	= tagSensorList.add_sensor_list();
		if (nullptr == pSensorData)
			continue;

		uCol	= 0;

		pResult->GetData(uRow, uCol++, uSensorID);
		pResult->GetData(uRow, uCol++, bySensorType);
		pResult->GetData(uRow, uCol++, bySensorState);
		pResult->GetData(uRow, uCol++, wSlopeID);
		pResult->GetData(uRow, uCol++, dLongitude);
		pResult->GetData(uRow, uCol++, dLatitude);
		pResult->GetData(uRow, uCol++, dCurValue1);
		pResult->GetData(uRow, uCol++, dCurValue2);
		pResult->GetData(uRow, uCol++, dCurValue3);
		pResult->GetData(uRow, uCol++, dAvgValue1);
		pResult->GetData(uRow, uCol++, dAvgValue2);
		pResult->GetData(uRow, uCol++, dAvgValue3);

		pSensorData->set_id(uSensorID);
		pSensorData->set_type(bySensorType);
		pSensorData->set_state(bySensorState);
		pSensorData->set_slope_id(wSlopeID);
		pSensorData->set_longitude(dLongitude);
		pSensorData->set_latitude(dLatitude);
		pSensorData->set_cur_value1(dCurValue1);
		pSensorData->set_cur_value2(dCurValue2);
		pSensorData->set_cur_value3(dCurValue3);
		pSensorData->set_avg_value1(dAvgValue1);
		pSensorData->set_avg_value2(dAvgValue2);
		pSensorData->set_avg_value3(dAvgValue3);
	}

	SendWebSensorList(tagSensorList);
}

void CWebClient::DBResopndSensorHistory(SMysqlRespond &pRespond, IQueryResult *pResult)
{
	WEB_SERVER_NET_Protocol::S2Web_Sensor_History	tagSensorHistory;

	UINT	uCol			= 0;
	BYTE	bySensorState	= 0;
	double	dLongitude		= 0.0f;
	double	dLatitude		= 0.0f;
	double	dCurValue1		= 0.0f;
	double	dCurValue2		= 0.0f;
	double	dCurValue3		= 0.0f;
	double	dOffsetValue1	= 0.0f;
	double	dOffsetValue2	= 0.0f;
	double	dOffsetValue3	= 0.0f;

	for (auto uRow = 0; uRow < pRespond.uRowCount; ++uRow)
	{
		WEB_SERVER_NET_Protocol::S2Web_Sensor_History::SensorData	*pSensorData	= tagSensorHistory.add_history_list();
		if (nullptr == pSensorData)
			continue;

		uCol	= 0;

		pResult->GetData(uRow, uCol++, bySensorState);
		pResult->GetData(uRow, uCol++, dLongitude);
		pResult->GetData(uRow, uCol++, dLatitude);
		pResult->GetData(uRow, uCol++, dCurValue1);
		pResult->GetData(uRow, uCol++, dCurValue2);
		pResult->GetData(uRow, uCol++, dCurValue3);
		pResult->GetData(uRow, uCol++, dOffsetValue1);
		pResult->GetData(uRow, uCol++, dOffsetValue2);
		pResult->GetData(uRow, uCol++, dOffsetValue3);

		pSensorData->set_state(bySensorState);
		pSensorData->set_value1(dCurValue1);
		pSensorData->set_value2(dCurValue2);
		pSensorData->set_value3(dCurValue3);
		pSensorData->set_offset_value1(dOffsetValue1);
		pSensorData->set_offset_value2(dOffsetValue2);
		pSensorData->set_offset_value3(dOffsetValue3);
	}

	// tagSensorHistory中还有一些数据未赋值，考虑一下，怎么从查询开始，经Mysql的Execute，再传到这里来
	// 如非必要，不要通过Mysql的Sql语句来
	// ...

	SendWebSensorHistory(tagSensorHistory);
}

void CWebClient::DBResopndAllList(SMysqlRespond &pRespond, IQueryResult *pResult)
{
}

void CWebClient::SendWebLoginResult(WEB_SERVER_NET_Protocol::S2Web_Login_Result &tagLoginResult)
{
}

void CWebClient::SendWebSlopeList(WEB_SERVER_NET_Protocol::S2Web_Slope_List &tagSlopeList)
{
}

void CWebClient::SendWebSensorList(WEB_SERVER_NET_Protocol::S2Web_Sensor_List &tagSensorList)
{
}

void CWebClient::SendWebSensorHistory(WEB_SERVER_NET_Protocol::S2Web_Sensor_History &tagSensorHistory)
{
}
