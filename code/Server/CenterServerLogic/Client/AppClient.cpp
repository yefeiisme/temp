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

	pMysqlQuery->PrepareProc("AppLogin");
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

	if (tagRequestSensorList.sensor_type())
	{
		pMysqlQuery->PrepareProc("LoadSensorListByType");
		pMysqlQuery->AddParam(m_uAccountID);
		pMysqlQuery->AddParam(tagRequestSensorList.slope_id());
		pMysqlQuery->AddParam(tagRequestSensorList.sensor_type());
		pMysqlQuery->EndPrepareProc(&tagRequest, sizeof(tagRequest));
	}
	else
	{
		pMysqlQuery->PrepareProc("LoadSensorList");
		pMysqlQuery->AddParam(m_uAccountID);
		pMysqlQuery->AddParam(tagRequestSensorList.slope_id());
		pMysqlQuery->EndPrepareProc(&tagRequest, sizeof(tagRequest));
	}

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

	uCol	= 0;
	pResult1->GetData(0, uCol++, m_uAccountID);

	APP_SERVER_NET_Protocol::S2App_Login_Result	tagLoginResult;

	for (auto uRow = 0; uRow < pResult2->GetRowCount(); ++uRow)
	{
		APP_SERVER_NET_Protocol::S2App_Login_Result::ServerData	*pServerData = tagLoginResult.add_server_list();
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

	SendAppLoginResult(tagLoginResult);
}

void CAppClient::DBResopndSlopeList(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData)
{
	UINT	uCol				= 0;
	WORD	wSlopeID			= 0;
	BYTE	bySlopeType			= 0;
	char	strSlopeName[64]	= {0};
	double	dLongitude			= 0.0f;
	double	dLatitude			= 0.0f;
	BYTE	byState				= 0;

	BYTE	byResultCount = pResultSet->GetResultCount();
	if (1 != byResultCount)
	{
		g_pFileLog->WriteLog("[%s][%d] Result Count[%hhu] Error\n", __FILE__, __LINE__, byResultCount);
		return;
	}

	IMysqlResult	*pResult1 = pResultSet->GetMysqlResult(0);

	if (nullptr == pResult1)
	{
		return;
	}

	if (1 != pResult1->GetRowCount())
	{
		return;
	}

	APP_SERVER_NET_Protocol::S2App_Slope_List	tagSlopeList;

	for (auto uRow = 0; uRow < pResult1->GetRowCount(); ++uRow)
	{
		APP_SERVER_NET_Protocol::S2App_Slope_List::SlopeData	*pSlopeData = tagSlopeList.add_slope_list();
		if (nullptr == pSlopeData)
			continue;

		uCol	= 0;

		pResult1->GetData(uRow, uCol++, wSlopeID);
		pResult1->GetData(uRow, uCol++, bySlopeType);
		pResult1->GetData(uRow, uCol++, strSlopeName, sizeof(strSlopeName));
		pResult1->GetData(uRow, uCol++, dLongitude);
		pResult1->GetData(uRow, uCol++, dLatitude);
		pResult1->GetData(uRow, uCol++, byState);

		pSlopeData->set_id(wSlopeID);
		pSlopeData->set_type(bySlopeType);
		pSlopeData->set_name(strSlopeName);
		pSlopeData->set_longitude(dLongitude);
		pSlopeData->set_latitude(dLatitude);
		pSlopeData->set_state(byState);
	}

	SendAppSlopeList(tagSlopeList);
}

void CAppClient::DBResopndSensorList(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData)
{
	UINT	uCol				= 0;
	UINT	uSensorID			= 0;
	BYTE	bySensorType		= 0;
	double	dCurValue1			= 0.0f;
	double	dCurValue2			= 0.0f;
	double	dCurValue3			= 0.0f;
	double	dAvgValue1			= 0.0f;
	double	dAvgValue2			= 0.0f;
	double	dAvgValue3			= 0.0f;
	double	dOffsetValue1		= 0.0f;
	double	dOffsetValue2		= 0.0f;
	double	dOffsetValue3		= 0.0f;
	BYTE	byState				= 0;
	WORD	wSlopeID			= 0;
	double	dLongitude			= 0.0f;
	double	dLatitude			= 0.0f;

	BYTE	byResultCount = pResultSet->GetResultCount();
	if (1 != byResultCount)
	{
		g_pFileLog->WriteLog("[%s][%d] Result Count[%hhu] Error\n", __FILE__, __LINE__, byResultCount);
		return;
	}

	IMysqlResult	*pResult1 = pResultSet->GetMysqlResult(0);

	if (nullptr == pResult1)
	{
		return;
	}

	if (1 != pResult1->GetRowCount())
	{
		return;
	}

	APP_SERVER_NET_Protocol::S2App_Sensor_History	tagSensorHistory;

	for (auto uRow = 0; uRow < pResult1->GetRowCount(); ++uRow)
	{
		APP_SERVER_NET_Protocol::S2App_Sensor_History::SensorData	*pSensor = tagSensorHistory.add_history_list();
		if (nullptr == pSensor)
			continue;

		uCol	= 0;

		//pResult1->GetData(uRow, uCol++, uSensorID);
		//pResult1->GetData(uRow, uCol++, bySensorType);
		//pResult1->GetData(uRow, uCol++, dCurValue1);
		//pResult1->GetData(uRow, uCol++, dCurValue2);
		//pResult1->GetData(uRow, uCol++, dCurValue3);
		//pResult1->GetData(uRow, uCol++, dAvgValue1);
		//pResult1->GetData(uRow, uCol++, dAvgValue2);
		//pResult1->GetData(uRow, uCol++, dAvgValue3);
		//pResult1->GetData(uRow, uCol++, byState);
		//pResult1->GetData(uRow, uCol++, wSlopeID);
		//pResult1->GetData(uRow, uCol++, dLongitude);
		//pResult1->GetData(uRow, uCol++, dLatitude);

		//pSensor->set_id(uSensorID);
		//pSensor->set_type(bySensorType);
		//pSensor->set_cur_value1(dCurValue1);
		//pSensor->set_cur_value2(dCurValue2);
		//pSensor->set_cur_value3(dCurValue3);
		//pSensor->set_avg_value1(dAvgValue1);
		//pSensor->set_avg_value2(dAvgValue2);
		//pSensor->set_avg_value3(dAvgValue3);
		//pSensor->set_state(byState);
		//pSensor->set_slope_id(wSlopeID);
		//pSensor->set_longitude(dLongitude);
		//pSensor->set_latitude(dLatitude);
	}

	SendAppSensorHistory(tagSensorHistory);
}

void CAppClient::DBResopndSensorHistory(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData)
{
	UINT	uCol				= 0;
	UINT	uSensorID			= 0;
	BYTE	bySensorType		= 0;
	double	dCurValue1			= 0.0f;
	double	dCurValue2			= 0.0f;
	double	dCurValue3			= 0.0f;
	double	dAvgValue1			= 0.0f;
	double	dAvgValue2			= 0.0f;
	double	dAvgValue3			= 0.0f;
	double	dOffsetValue1		= 0.0f;
	double	dOffsetValue2		= 0.0f;
	double	dOffsetValue3		= 0.0f;
	BYTE	byState				= 0;
	WORD	wSlopeID			= 0;
	double	dLongitude			= 0.0f;
	double	dLatitude			= 0.0f;

	BYTE	byResultCount = pResultSet->GetResultCount();
	if (1 != byResultCount)
	{
		g_pFileLog->WriteLog("[%s][%d] Result Count[%hhu] Error\n", __FILE__, __LINE__, byResultCount);
		return;
	}

	IMysqlResult	*pResult1 = pResultSet->GetMysqlResult(0);

	if (nullptr == pResult1)
	{
		return;
	}

	if (1 != pResult1->GetRowCount())
	{
		return;
	}

	APP_SERVER_NET_Protocol::S2App_Sensor_List	tagSensorList;

	for (auto uRow = 0; uRow < pResult1->GetRowCount(); ++uRow)
	{
		APP_SERVER_NET_Protocol::S2App_Sensor_List::SensorData	*pSensor = tagSensorList.add_sensor_list();
		if (nullptr == pSensor)
			continue;

		uCol	= 0;

		pResult1->GetData(uRow, uCol++, uSensorID);
		pResult1->GetData(uRow, uCol++, bySensorType);
		pResult1->GetData(uRow, uCol++, dCurValue1);
		pResult1->GetData(uRow, uCol++, dCurValue2);
		pResult1->GetData(uRow, uCol++, dCurValue3);
		pResult1->GetData(uRow, uCol++, dAvgValue1);
		pResult1->GetData(uRow, uCol++, dAvgValue2);
		pResult1->GetData(uRow, uCol++, dAvgValue3);
		pResult1->GetData(uRow, uCol++, byState);
		pResult1->GetData(uRow, uCol++, wSlopeID);
		pResult1->GetData(uRow, uCol++, dLongitude);
		pResult1->GetData(uRow, uCol++, dLatitude);

		pSensor->set_id(uSensorID);
		pSensor->set_type(bySensorType);
		pSensor->set_cur_value1(dCurValue1);
		pSensor->set_cur_value2(dCurValue2);
		pSensor->set_cur_value3(dCurValue3);
		pSensor->set_avg_value1(dAvgValue1);
		pSensor->set_avg_value2(dAvgValue2);
		pSensor->set_avg_value3(dAvgValue3);
		pSensor->set_state(byState);
		pSensor->set_slope_id(wSlopeID);
		pSensor->set_longitude(dLongitude);
		pSensor->set_latitude(dLatitude);
	}

	SendAppSensorList(tagSensorList);
}

void CAppClient::DBResopndAllList(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData)
{
}

void CAppClient::SendAppLoginResult(APP_SERVER_NET_Protocol::S2App_Login_Result &tagLoginResult)
{
	char	strBuffer[0xffff]	= {0};
	if (sizeof(BYTE)+tagLoginResult.ByteSize() > sizeof(strBuffer))
		return;

	*(BYTE*)strBuffer	= APP_SERVER_NET_Protocol::S2APP::s2app_login_result;
	tagLoginResult.SerializeToArray(strBuffer + sizeof(BYTE), tagLoginResult.ByteSize());

	m_pClientConn->PutPack(strBuffer, sizeof(BYTE)+tagLoginResult.ByteSize());
}

void CAppClient::SendAppSlopeList(APP_SERVER_NET_Protocol::S2App_Slope_List &tagSlopeList)
{
	char	strBuffer[0xffff]	= {0};
	if (sizeof(BYTE)+tagSlopeList.ByteSize() > sizeof(strBuffer))
		return;

	*(BYTE*)strBuffer	= APP_SERVER_NET_Protocol::S2APP::s2app_slope_list;
	tagSlopeList.SerializeToArray(strBuffer + sizeof(BYTE), tagSlopeList.ByteSize());

	m_pClientConn->PutPack(strBuffer, sizeof(BYTE)+tagSlopeList.ByteSize());
}

void CAppClient::SendAppSensorList(APP_SERVER_NET_Protocol::S2App_Sensor_List &tagSensorList)
{
	char	strBuffer[0xffff]	= {0};
	if (sizeof(BYTE)+tagSensorList.ByteSize() > sizeof(strBuffer))
		return;

	*(BYTE*)strBuffer	= APP_SERVER_NET_Protocol::S2APP::s2app_sensor_list;
	tagSensorList.SerializeToArray(strBuffer + sizeof(BYTE), tagSensorList.ByteSize());

	m_pClientConn->PutPack(strBuffer, sizeof(BYTE)+tagSensorList.ByteSize());
}

void CAppClient::SendAppSensorHistory(APP_SERVER_NET_Protocol::S2App_Sensor_History &tagSensorHistory)
{
	char	strBuffer[0xffff]	= {0};
	if (sizeof(BYTE)+tagSensorHistory.ByteSize() > sizeof(strBuffer))
		return;

	*(BYTE*)strBuffer	= APP_SERVER_NET_Protocol::S2APP::s2app_sensor_history;
	tagSensorHistory.SerializeToArray(strBuffer + sizeof(BYTE), tagSensorHistory.ByteSize());

	m_pClientConn->PutPack(strBuffer, sizeof(BYTE)+tagSensorHistory.ByteSize());
}
