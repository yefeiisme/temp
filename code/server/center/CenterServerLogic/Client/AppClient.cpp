#include "stdafx.h"
#include "AppClient.h"
#include "ICenterServer.h"
#include "IMysqlQuery.h"
#include "../SensorDB/SensorDBOperation.h"
#include "CommonDefine.pb.h"

CAppClient::pfnProtocolFunc CAppClient::m_ProtocolFunc[APP_SERVER_NET_Protocol::APP2S::app2s_max] =
{
	&CAppClient::RecvLogin,
	&CAppClient::RecvRequestSlopeList,
	&CAppClient::RecvRequestSensorList,
	&CAppClient::RecvRequestSensorHistory,
	&CAppClient::RecvPing,

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
	{
		APP_SERVER_NET_Protocol::S2APP_ERROR	tagError;
		tagError.set_error_code(CommonDefine::ERROR_CODE::ec_please_login);

		SendAppMsg(APP_SERVER_NET_Protocol::S2APP::s2app_error, tagError);

		return;
	}

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
		pMysqlQuery->AddParam(tagRequestSensorList.slope_id());
		pMysqlQuery->AddParam(tagRequestSensorList.sensor_type());
		pMysqlQuery->EndPrepareProc(&tagRequest, sizeof(tagRequest));
	}
	else
	{
		pMysqlQuery->PrepareProc("LoadSensorList");
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
	tagRequest.byOpt			= SENSOR_DB_SENSOR_HISTORY;
	tagRequest.uClientID		= m_uUniqueID;
	tagRequest.uClientIndex		= m_uIndex;
	tagRequest.byClientType		= APP_CLIENT;

	IMysqlQuery	*pMysqlQuery	= g_ICenterServer.GetMysqlQuery();
	if (nullptr == pMysqlQuery)
		return;

	pMysqlQuery->PrepareProc("LoadSensorHistory");
	pMysqlQuery->AddParam(tagRequestSensorHistory.sensor_id());
	pMysqlQuery->AddParam(tagRequestSensorHistory.begin_time());
	pMysqlQuery->AddParam(tagRequestSensorHistory.end_time());
	pMysqlQuery->EndPrepareProc(&tagRequest, sizeof(tagRequest));

	pMysqlQuery->CallProc();
}

void CAppClient::RecvPing(const void *pPack, const unsigned int uPackLen)
{
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
	UINT	uCol			= 0;
	UINT	uAccountCount	= 0;
	UINT	uServerID		= 0;
	WORD	wServerPort		= 0;
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
	pResult1->GetData(0, uCol++, uAccountCount);

	APP_SERVER_NET_Protocol::S2App_Login_Result	tagLoginResult;

	if (1 == uAccountCount)
	{
		tagLoginResult.set_result(0);
	}
	else
	{
		tagLoginResult.set_result(1);
	}

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

	SendAppMsg(APP_SERVER_NET_Protocol::S2APP::s2app_login_result, tagLoginResult);
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
	char	strUrl[0xffff]		= {0};

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

	if (0 == pResult1->GetRowCount())
	{
		g_pFileLog->WriteLog("[%s][%d] Result1 Row Count 0\n", __FILE__, __LINE__);
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
		pResult1->GetData(uRow, uCol++, strUrl, sizeof(strUrl));

		pSlopeData->set_id(wSlopeID);
		pSlopeData->set_type(bySlopeType);
		pSlopeData->set_name(strSlopeName);
		pSlopeData->set_longitude(dLongitude);
		pSlopeData->set_latitude(dLatitude);
		pSlopeData->set_state(byState);
	}

	SendAppMsg(APP_SERVER_NET_Protocol::S2APP::s2app_slope_list, tagSlopeList);
}

void CAppClient::DBResopndSensorList(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData)
{
	UINT	uCol			= 0;
	UINT	uSensorID		= 0;
	BYTE	bySensorType	= 0;
	double	dCurValue1		= 0.0f;
	double	dCurValue2		= 0.0f;
	double	dCurValue3		= 0.0f;
	double	dAvgValue1		= 0.0f;
	double	dAvgValue2		= 0.0f;
	double	dAvgValue3		= 0.0f;
	double	dOffsetValue1	= 0.0f;
	double	dOffsetValue2	= 0.0f;
	double	dOffsetValue3	= 0.0f;
	BYTE	byState			= 0;
	WORD	wSlopeID		= 0;
	double	dLongitude		= 0.0f;
	double	dLatitude		= 0.0f;
	char	strUrl[0xffff]	= {0};
	char	strDesc[0xffff]	= {0};

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

	if (0 == pResult1->GetRowCount())
	{
		g_pFileLog->WriteLog("CAppClient::DBResopndSensorList Return Row Count 0\n", __FILE__, __LINE__);
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
		pResult1->GetData(uRow, uCol++, dOffsetValue1);
		pResult1->GetData(uRow, uCol++, dOffsetValue2);
		pResult1->GetData(uRow, uCol++, dOffsetValue3);
		pResult1->GetData(uRow, uCol++, byState);
		pResult1->GetData(uRow, uCol++, wSlopeID);
		pResult1->GetData(uRow, uCol++, dLongitude);
		pResult1->GetData(uRow, uCol++, dLatitude);
		pResult1->GetData(uRow, uCol++, strUrl, sizeof(strUrl));
		pResult1->GetData(uRow, uCol++, strDesc, sizeof(strDesc));

		pSensor->set_id(uSensorID);
		pSensor->set_type(bySensorType);
		pSensor->set_cur_value1(dCurValue1);
		pSensor->set_cur_value2(dCurValue2);
		pSensor->set_cur_value3(dCurValue3);
		pSensor->set_avg_value1(dAvgValue1);
		pSensor->set_avg_value2(dAvgValue2);
		pSensor->set_avg_value3(dAvgValue3);
		pSensor->set_offset_value1(dOffsetValue1);
		pSensor->set_offset_value2(dOffsetValue2);
		pSensor->set_offset_value3(dOffsetValue3);
		pSensor->set_state(byState);
		pSensor->set_slope_id(wSlopeID);
		pSensor->set_longitude(dLongitude);
		pSensor->set_latitude(dLatitude);
		pSensor->set_url(strUrl);
		pSensor->set_description(strDesc);
	}

	SendAppMsg(APP_SERVER_NET_Protocol::S2APP::s2app_sensor_list, tagSensorList);
}

void CAppClient::DBResopndSensorHistory(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData)
{
	UINT	uCol		= 0;
	UINT	uSensorID	= 0;
	double	dLongitude	= 0.0f;
	double	dLatitude	= 0.0f;
	int		nBeginTime	= 0;
	int		nEndTime	= 0;
	int		nDataTime	= 0;
	UINT	uInterval	= 0;
	double	dAvgValue1	= 0.0f;
	double	dAvgValue2	= 0.0f;
	double	dAvgValue3	= 0.0f;

	WORD	wSlopeID	= 0;
	double	dMinValue1	= 0.0f;
	double	dMinValue2	= 0.0f;
	double	dMinValue3	= 0.0f;
	double	dMaxValue1	= 0.0f;
	double	dMaxValue2	= 0.0f;
	double	dMaxValue3	= 0.0f;

	BYTE	byResultCount = pResultSet->GetResultCount();
	if (2 != byResultCount)
	{
		g_pFileLog->WriteLog("[%s][%d] Result Count[%hhu] Error\n", __FILE__, __LINE__, byResultCount);
		return;
	}

	IMysqlResult	*pResult1	= pResultSet->GetMysqlResult(0);
	IMysqlResult	*pResult2	= pResultSet->GetMysqlResult(1);

	if (nullptr == pResult1 || nullptr == pResult2)
	{
		return;
	}

	if (1 != pResult1->GetRowCount())
	{
		return;
	}

	APP_SERVER_NET_Protocol::S2App_Sensor_History	tagSensorHistory;

	uCol	= 0;

	pResult1->GetData(0, uCol++, uSensorID);
	pResult1->GetData(0, uCol++, dLongitude);
	pResult1->GetData(0, uCol++, dLatitude);
	pResult1->GetData(0, uCol++, nBeginTime);
	pResult1->GetData(0, uCol++, nEndTime);
	pResult1->GetData(0, uCol++, uInterval);
	pResult1->GetData(0, uCol++, dAvgValue1);
	pResult1->GetData(0, uCol++, dAvgValue2);
	pResult1->GetData(0, uCol++, dAvgValue3);

	tagSensorHistory.set_id(uSensorID);
	tagSensorHistory.set_longitude(dLongitude);
	tagSensorHistory.set_latitude(dLatitude);
	tagSensorHistory.set_begin_time(nBeginTime);
	tagSensorHistory.set_end_time(nEndTime);
	tagSensorHistory.set_invterval(uInterval);
	tagSensorHistory.set_avg_value1(dAvgValue1);
	tagSensorHistory.set_avg_value2(dAvgValue2);
	tagSensorHistory.set_avg_value3(dAvgValue3);

	for (auto uRow = 0; uRow < pResult2->GetRowCount(); ++uRow)
	{
		APP_SERVER_NET_Protocol::S2App_Sensor_History::SensorData	*pSensor = tagSensorHistory.add_history_list();
		if (nullptr == pSensor)
			continue;

		uCol	= 0;

		pResult2->GetData(uRow, uCol++, dMinValue1);
		pResult2->GetData(uRow, uCol++, dMinValue2);
		pResult2->GetData(uRow, uCol++, dMinValue3);
		pResult2->GetData(uRow, uCol++, dMaxValue1);
		pResult2->GetData(uRow, uCol++, dMaxValue2);
		pResult2->GetData(uRow, uCol++, dMaxValue3);

		pSensor->set_min_value1(dMinValue1);
		pSensor->set_min_value2(dMinValue2);
		pSensor->set_min_value3(dMinValue3);
		pSensor->set_max_value1(dMaxValue1);
		pSensor->set_max_value2(dMaxValue2);
		pSensor->set_max_value3(dMaxValue3);
	}

	SendAppMsg(APP_SERVER_NET_Protocol::S2APP::s2app_sensor_history, tagSensorHistory);
}

void CAppClient::DBResopndAllList(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData)
{
	UINT	uCol				= 0;
	WORD	wSlopeID			= 0;
	BYTE	bySlopeType			= 0;
	char	strSlopeName[128]	= {0};
	double	dSlopeLongitude		= 0.0f;
	double	dSlopeLatitude		= 0.0f;
	BYTE	bySlopeState		= 0;

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
	BYTE	bySensorState		= 0;
	double	dLongitude			= 0.0f;
	double	dLatitude			= 0.0f;
	char	strSensorUrl[256]	= {0};

	BYTE	byResultCount = pResultSet->GetResultCount();
	if (2 != byResultCount)
	{
		g_pFileLog->WriteLog("[%s][%d] Result Count[%hhu] Error\n", __FILE__, __LINE__, byResultCount);
		return;
	}

	IMysqlResult	*pResult1	= pResultSet->GetMysqlResult(0);
	IMysqlResult	*pResult2	= pResultSet->GetMysqlResult(1);

	if (nullptr == pResult1 || nullptr == pResult2)
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
		pResult1->GetData(uRow, uCol++, bySlopeState);

		pSlopeData->set_id(wSlopeID);
		pSlopeData->set_type(bySlopeType);
		pSlopeData->set_name(strSlopeName);
		pSlopeData->set_longitude(dLongitude);
		pSlopeData->set_latitude(dLatitude);
		pSlopeData->set_state(bySlopeState);
	}

	SendAppMsg(APP_SERVER_NET_Protocol::S2APP::s2app_slope_list, tagSlopeList);

	// 后面添加sensor数据的读取
	// ...
}

void CAppClient::SendAppMsg(const BYTE byProtocol, google::protobuf::Message &tagMsg)
{
	char	strBuffer[0xffff]	= {0};
	if (sizeof(BYTE)+tagMsg.ByteSize() > sizeof(strBuffer))
		return;

	*(BYTE*)strBuffer	= byProtocol;
	tagMsg.SerializeToArray(strBuffer + sizeof(BYTE), tagMsg.ByteSize());

	m_pClientConn->PutPack(strBuffer, sizeof(BYTE)+tagMsg.ByteSize());
}
