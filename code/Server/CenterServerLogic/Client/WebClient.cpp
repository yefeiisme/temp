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

void CWebClient::ProcessDBPack(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData)
{
	if (pCallbackData->byOpt >= SENSOR_DB_OPT_MAX)
	{
		g_pFileLog->WriteLog("[%s][%d] DB Respond Invalid Protocol[%hhu]\n", __FILE__, __LINE__, pCallbackData->byOpt);
		return;
	}

	(this->*m_pfnDBRespondFunc[pCallbackData->byOpt])(pResultSet, pCallbackData);
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

	pMysqlQuery->PrepareProc("WebLogin");
	pMysqlQuery->AddParam(tagLoginInfo.account().c_str());
	pMysqlQuery->AddParam(tagLoginInfo.password().c_str());
	pMysqlQuery->EndPrepareProc(&tagRequest, sizeof(tagRequest));

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
	pMysqlQuery->EndPrepareProc(&tagRequest, sizeof(tagRequest));

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
	pMysqlQuery->EndPrepareProc(&tagRequest, sizeof(tagRequest));

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
	pMysqlQuery->EndPrepareProc(&tagRequest, sizeof(tagRequest));

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
	pMysqlQuery->EndPrepareProc(&tagRequest, sizeof(tagRequest));

	pMysqlQuery->CallProc();
}

void CWebClient::DBResopndLoginResult(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData)
{
	UINT	uCol		= 0;
	UINT	uServerID	= 0;
	WORD	wServerPort	= 0;
	char	strServerIP[16];

	BYTE	byResultCount = pResultSet->GetResultCount();
	if (2 != byResultCount)
	{
		g_pFileLog->WriteLog("[%s][%d] Result Count[%hhu] Error\n", __FILE__, __LINE__, byResultCount);
		return;
	}

	IMysqlResult	*pResult1 = pResultSet->GetMysqlResult(0);
	IMysqlResult	*pResult2 = pResultSet->GetMysqlResult(1);

	if (nullptr == pResult1 || nullptr == pResult2)
	{
		return;
	}

	if (1 != pResult1->GetRowCount())
	{
		return;
	}

	WEB_SERVER_NET_Protocol::S2Web_Login_Result	tagLoginResult;

	for (auto uRow = 0; uRow < pResult2->GetRowCount(); ++uRow)
	{
		WEB_SERVER_NET_Protocol::S2Web_Login_Result::ServerData	*pServerData = tagLoginResult.add_server_list();
		if (nullptr == pServerData)
			continue;

		uCol	= 0;

		pResult2->GetData(uRow, uCol++, uServerID);
		pResult2->GetData(uRow, uCol++, strServerIP, sizeof(strServerIP));
		pResult2->GetData(uRow, uCol++, wServerPort);

		pServerData->set_id(uServerID);
		pServerData->set_ip(strServerIP);
		pServerData->set_port(wServerPort);
	}

	SendWebLoginResult(tagLoginResult);
}

void CWebClient::DBResopndSlopeList(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData)
{
	/*
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
	*/
}

void CWebClient::DBResopndSensorList(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData)
{
	/*
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
	*/
}

void CWebClient::DBResopndSensorHistory(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData)
{
	WEB_SERVER_NET_Protocol::S2Web_Sensor_History	tagSensorHistory;

	UINT	uCol			= 0;
	BYTE	bySensorState	= 0;
	double	dCurValue1		= 0.0f;
	double	dCurValue2		= 0.0f;
	double	dCurValue3		= 0.0f;
	double	dOffsetValue1	= 0.0f;
	double	dOffsetValue2	= 0.0f;
	double	dOffsetValue3	= 0.0f;

	UINT	uSensorID		= 0;
	double	dLongitude		= 0.0f;
	double	dLatitude		= 0.0f;
	int		nBeginTime		= 0;
	int		nEndTime		= 0;
	double	dAvgValue1		= 0.0f;
	double	dAvgValue2		= 0.0f;
	double	dAvgValue3		= 0.0f;


	BYTE	byResultCount = pResultSet->GetResultCount();
	if (3 != byResultCount)
	{
		g_pFileLog->WriteLog("[%s][%d] Result Count[%hhu] Error\n", __FILE__, __LINE__, byResultCount);
		return;
	}

	IMysqlResult	*pResult1 = pResultSet->GetMysqlResult(0);
	IMysqlResult	*pResult2 = pResultSet->GetMysqlResult(1);

	if (nullptr == pResult1 || nullptr == pResult2)
	{
		return;
	}

	for (auto uRow = 0; uRow < pResult1->GetRowCount(); ++uRow)
	{
		WEB_SERVER_NET_Protocol::S2Web_Sensor_History::SensorData	*pSensorData	= tagSensorHistory.add_history_list();
		if (nullptr == pSensorData)
			continue;

		uCol	= 0;

		pResult1->GetData(uRow, uCol++, bySensorState);
		pResult1->GetData(uRow, uCol++, dCurValue1);
		pResult1->GetData(uRow, uCol++, dCurValue2);
		pResult1->GetData(uRow, uCol++, dCurValue3);
		pResult1->GetData(uRow, uCol++, dOffsetValue1);
		pResult1->GetData(uRow, uCol++, dOffsetValue2);
		pResult1->GetData(uRow, uCol++, dOffsetValue3);

		pSensorData->set_state(bySensorState);
		pSensorData->set_value1(dCurValue1);
		pSensorData->set_value2(dCurValue2);
		pSensorData->set_value3(dCurValue3);
		pSensorData->set_offset_value1(dOffsetValue1);
		pSensorData->set_offset_value2(dOffsetValue2);
		pSensorData->set_offset_value3(dOffsetValue3);
	}

	uCol = 0;

	pResult2->GetData(0, uCol++, uSensorID);
	pResult2->GetData(0, uCol++, dLongitude);
	pResult2->GetData(0, uCol++, dLatitude);
	pResult2->GetData(0, uCol++, nBeginTime);
	pResult2->GetData(0, uCol++, nEndTime);
	pResult2->GetData(0, uCol++, dAvgValue1);
	pResult2->GetData(0, uCol++, dAvgValue2);
	pResult2->GetData(0, uCol++, dAvgValue3);

	tagSensorHistory.set_id(uSensorID);
	tagSensorHistory.set_longitude(dLongitude);
	tagSensorHistory.set_latitude(dLatitude);
	tagSensorHistory.set_begin_time(nBeginTime);
	tagSensorHistory.set_end_time(nEndTime);
	tagSensorHistory.set_avg_value1(dAvgValue1);
	tagSensorHistory.set_avg_value2(dAvgValue2);
	tagSensorHistory.set_avg_value3(dAvgValue3);

	// 如果有可能的话，这里再取一个result
	// ...
	SendWebSensorHistory(tagSensorHistory);
}

void CWebClient::DBResopndAllList(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData)
{
}

void CWebClient::SendWebLoginResult(WEB_SERVER_NET_Protocol::S2Web_Login_Result &tagLoginResult)
{
	char	strBuffer[0xffff]	= {0};
	if (sizeof(BYTE)+tagLoginResult.ByteSize() > sizeof(strBuffer))
		return;

	*(BYTE*)strBuffer	= WEB_SERVER_NET_Protocol::S2WEB::s2web_login_result;
	tagLoginResult.SerializeToArray(strBuffer+sizeof(BYTE), tagLoginResult.ByteSize());

	m_pClientConn->PutPack(strBuffer, sizeof(BYTE)+tagLoginResult.ByteSize());
}

void CWebClient::SendWebSlopeList(WEB_SERVER_NET_Protocol::S2Web_Slope_List &tagSlopeList)
{
	char	strBuffer[0xffff]	={ 0 };
	if (sizeof(BYTE)+tagSlopeList.ByteSize() > sizeof(strBuffer))
		return;

	*(BYTE*)strBuffer	= WEB_SERVER_NET_Protocol::S2WEB::s2web_slope_list;
	tagSlopeList.SerializeToArray(strBuffer + sizeof(BYTE), tagSlopeList.ByteSize());

	m_pClientConn->PutPack(strBuffer, sizeof(BYTE)+tagSlopeList.ByteSize());
}

void CWebClient::SendWebSensorList(WEB_SERVER_NET_Protocol::S2Web_Sensor_List &tagSensorList)
{
	char	strBuffer[0xffff]	={ 0 };
	if (sizeof(BYTE)+tagSensorList.ByteSize() > sizeof(strBuffer))
		return;

	*(BYTE*)strBuffer	= WEB_SERVER_NET_Protocol::S2WEB::s2web_sensor_list;
	tagSensorList.SerializeToArray(strBuffer + sizeof(BYTE), tagSensorList.ByteSize());

	m_pClientConn->PutPack(strBuffer, sizeof(BYTE)+tagSensorList.ByteSize());
}

void CWebClient::SendWebSensorHistory(WEB_SERVER_NET_Protocol::S2Web_Sensor_History &tagSensorHistory)
{
	char	strBuffer[0xffff]	={ 0 };
	if (sizeof(BYTE)+tagSensorHistory.ByteSize() > sizeof(strBuffer))
		return;

	*(BYTE*)strBuffer	= WEB_SERVER_NET_Protocol::S2WEB::s2web_sensor_history;
	tagSensorHistory.SerializeToArray(strBuffer + sizeof(BYTE), tagSensorHistory.ByteSize());

	m_pClientConn->PutPack(strBuffer, sizeof(BYTE)+tagSensorHistory.ByteSize());
}
