#include "stdafx.h"
#include "WebClient.h"
#include "ICenterServer.h"
#include "IMysqlQuery.h"
#include "../SensorDB/SensorDBOperation.h"
#include "CommonDefine.pb.h"

CWebClient::pfnProtocolFunc CWebClient::m_ProtocolFunc[WEB_SERVER_NET_Protocol::WEB2S::web2s_max] =
{
	&CWebClient::RecvLogin,
	&CWebClient::RecvRequestSlopeList,
	&CWebClient::RecvRequestSensorList,
	&CWebClient::RecvRequestSensorHistory,
	&CWebClient::RecvPing,

	&CWebClient::RecvRequestAllList,
	&CWebClient::RecvAddSlope,
	&CWebClient::RecvDelSlope,
	&CWebClient::RecvUpdateSlope,
	&CWebClient::RecvAddSensor,

	&CWebClient::RecvDelSensor,
	&CWebClient::RecvUpdateSensor,
	&CWebClient::RecvModifyPassword,
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

	&CWebClient::DBResopndAddSlopeResult,
	&CWebClient::DBResopndDelSlopeResult,
	&CWebClient::DBResopndUpdateSlopeResult,
	&CWebClient::DBResopndAddSensorResult,
	&CWebClient::DBResopndDelSensorResult,

	&CWebClient::DBResopndUpdateSensorResult,
	&CWebClient::DBResopndModifyPasswordResult,
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
	{
		WEB_SERVER_NET_Protocol::S2WEB_ERROR	tagError;
		tagError.set_error_code(CommonDefine::ERROR_CODE::ec_please_login);

		SendWebMsg(WEB_SERVER_NET_Protocol::S2WEB::s2web_error, tagError);

		return;
	}

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
	{
		WEB_SERVER_NET_Protocol::S2WEB_ERROR	tagError;
		tagError.set_error_code(CommonDefine::ERROR_CODE::ec_please_login);

		SendWebMsg(WEB_SERVER_NET_Protocol::S2WEB::s2web_error, tagError);

		return;
	}

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
	pMysqlQuery->AddParam(tagRequestSensorList.slope_id());
	pMysqlQuery->EndPrepareProc(&tagRequest, sizeof(tagRequest));

	pMysqlQuery->CallProc();
}

void CWebClient::RecvRequestSensorHistory(const void *pPack, const unsigned int uPackLen)
{
	if (0 == m_uAccountID)
	{
		WEB_SERVER_NET_Protocol::S2WEB_ERROR	tagError;
		tagError.set_error_code(CommonDefine::ERROR_CODE::ec_please_login);

		SendWebMsg(WEB_SERVER_NET_Protocol::S2WEB::s2web_error, tagError);

		return;
	}

	WEB_SERVER_NET_Protocol::WEB2S_Request_Sensor_History	tagRequestSensorHistory;
	BYTE	*pLoginInfo = (BYTE*)pPack + sizeof(BYTE);
	tagRequestSensorHistory.ParseFromArray(pLoginInfo, uPackLen - sizeof(BYTE));

	SMysqlRequest	tagRequest	= {0};
	tagRequest.byOpt			= SENSOR_DB_SENSOR_HISTORY;
	tagRequest.uClientID		= m_uUniqueID;
	tagRequest.uClientIndex		= m_uIndex;
	tagRequest.byClientType		= WEB_CLIENT;

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

void CWebClient::RecvPing(const void *pPack, const unsigned int uPackLen)
{
}

void CWebClient::RecvRequestAllList(const void *pPack, const unsigned int uPackLen)
{
}

void CWebClient::RecvAddSlope(const void *pPack, const unsigned int uPackLen)
{
	if (0 == m_uAccountID)
	{
		WEB_SERVER_NET_Protocol::S2WEB_ERROR	tagError;
		tagError.set_error_code(CommonDefine::ERROR_CODE::ec_please_login);

		SendWebMsg(WEB_SERVER_NET_Protocol::S2WEB::s2web_error, tagError);

		return;
	}

	WEB_SERVER_NET_Protocol::WEB2S_Add_Slope	tagAddSlope;
	BYTE	*pSlopeInfo = (BYTE*)pPack + sizeof(BYTE);
	tagAddSlope.ParseFromArray(pSlopeInfo, uPackLen - sizeof(BYTE));

	SMysqlRequest	tagRequest	= {0};
	tagRequest.byOpt			= SENSOR_DB_ADD_SLOPE;
	tagRequest.uClientID		= m_uUniqueID;
	tagRequest.uClientIndex		= m_uIndex;
	tagRequest.byClientType		= WEB_CLIENT;

	IMysqlQuery	*pMysqlQuery	= g_ICenterServer.GetMysqlQuery();
	if (nullptr == pMysqlQuery)
		return;

	pMysqlQuery->PrepareProc("AddSlope");
	pMysqlQuery->AddParam(tagAddSlope.type());
	pMysqlQuery->AddParam(tagAddSlope.name().c_str());
	pMysqlQuery->AddParam(tagAddSlope.longitude());
	pMysqlQuery->AddParam(tagAddSlope.latitude());
	pMysqlQuery->AddParam(m_uAccountID);
	pMysqlQuery->AddParam(tagAddSlope.url().c_str());
	pMysqlQuery->EndPrepareProc(&tagRequest, sizeof(tagRequest));

	pMysqlQuery->CallProc();
}

void CWebClient::RecvDelSlope(const void *pPack, const unsigned int uPackLen)
{
	if (0 == m_uAccountID)
	{
		WEB_SERVER_NET_Protocol::S2WEB_ERROR	tagError;
		tagError.set_error_code(CommonDefine::ERROR_CODE::ec_please_login);

		SendWebMsg(WEB_SERVER_NET_Protocol::S2WEB::s2web_error, tagError);

		return;
	}

	WEB_SERVER_NET_Protocol::WEB2S_Del_Slope	tagDelSlope;
	BYTE	*pSlopeInfo = (BYTE*)pPack + sizeof(BYTE);
	tagDelSlope.ParseFromArray(pSlopeInfo, uPackLen - sizeof(BYTE));

	SMysqlRequest	tagRequest	= {0};
	tagRequest.byOpt			= SENSOR_DB_DEL_SLOPE;
	tagRequest.uClientID		= m_uUniqueID;
	tagRequest.uClientIndex		= m_uIndex;
	tagRequest.byClientType		= WEB_CLIENT;

	IMysqlQuery	*pMysqlQuery	= g_ICenterServer.GetMysqlQuery();
	if (nullptr == pMysqlQuery)
		return;

	pMysqlQuery->PrepareProc("DeleteSlope");
	pMysqlQuery->AddParam(tagDelSlope.id());
	pMysqlQuery->EndPrepareProc(&tagRequest, sizeof(tagRequest));

	pMysqlQuery->CallProc();
}

void CWebClient::RecvUpdateSlope(const void *pPack, const unsigned int uPackLen)
{
	if (0 == m_uAccountID)
	{
		WEB_SERVER_NET_Protocol::S2WEB_ERROR	tagError;
		tagError.set_error_code(CommonDefine::ERROR_CODE::ec_please_login);

		SendWebMsg(WEB_SERVER_NET_Protocol::S2WEB::s2web_error, tagError);

		return;
	}

	WEB_SERVER_NET_Protocol::WEB2S_Update_Slope_Data	tagSlopeData;
	BYTE	*pSlopeInfo = (BYTE*)pPack + sizeof(BYTE);
	tagSlopeData.ParseFromArray(pSlopeInfo, uPackLen - sizeof(BYTE));

	SMysqlRequest	tagRequest	= {0};
	tagRequest.byOpt			= SENSOR_DB_UPDATE_SLOPE;
	tagRequest.uClientID		= m_uUniqueID;
	tagRequest.uClientIndex		= m_uIndex;
	tagRequest.byClientType		= WEB_CLIENT;

	IMysqlQuery	*pMysqlQuery	= g_ICenterServer.GetMysqlQuery();
	if (nullptr == pMysqlQuery)
		return;

	pMysqlQuery->PrepareProc("UpdateSlope");
	pMysqlQuery->AddParam(tagSlopeData.id());
	pMysqlQuery->AddParam(tagSlopeData.type());
	pMysqlQuery->AddParam(tagSlopeData.name().c_str());
	pMysqlQuery->AddParam(tagSlopeData.longitude());
	pMysqlQuery->AddParam(tagSlopeData.latitude());
	pMysqlQuery->AddParam(tagSlopeData.url().c_str());
	pMysqlQuery->EndPrepareProc(&tagRequest, sizeof(tagRequest));

	pMysqlQuery->CallProc();
}

void CWebClient::RecvAddSensor(const void *pPack, const unsigned int uPackLen)
{
	if (0 == m_uAccountID)
	{
		WEB_SERVER_NET_Protocol::S2WEB_ERROR	tagError;
		tagError.set_error_code(CommonDefine::ERROR_CODE::ec_please_login);

		SendWebMsg(WEB_SERVER_NET_Protocol::S2WEB::s2web_error, tagError);

		return;
	}

	WEB_SERVER_NET_Protocol::WEB2S_Add_Sensor	tagAddSensor;
	BYTE	*pSensorInfo = (BYTE*)pPack + sizeof(BYTE);
	tagAddSensor.ParseFromArray(pSensorInfo, uPackLen - sizeof(BYTE));

	SMysqlRequest	tagRequest	= {0};
	tagRequest.byOpt			= SENSOR_DB_ADD_SENSOR;
	tagRequest.uClientID		= m_uUniqueID;
	tagRequest.uClientIndex		= m_uIndex;
	tagRequest.byClientType		= WEB_CLIENT;

	IMysqlQuery	*pMysqlQuery	= g_ICenterServer.GetMysqlQuery();
	if (nullptr == pMysqlQuery)
		return;

	pMysqlQuery->PrepareProc("AddSensor");
	pMysqlQuery->AddParam(tagAddSensor.type());
	pMysqlQuery->AddParam(tagAddSensor.slope_id());
	pMysqlQuery->AddParam(tagAddSensor.longitude());
	pMysqlQuery->AddParam(tagAddSensor.latitude());
	pMysqlQuery->AddParam(tagAddSensor.url().c_str());
	pMysqlQuery->AddParam(tagAddSensor.description().c_str());
	pMysqlQuery->EndPrepareProc(&tagRequest, sizeof(tagRequest));

	pMysqlQuery->CallProc();
}


void CWebClient::RecvDelSensor(const void *pPack, const unsigned int uPackLen)
{
	if (0 == m_uAccountID)
	{
		WEB_SERVER_NET_Protocol::S2WEB_ERROR	tagError;
		tagError.set_error_code(CommonDefine::ERROR_CODE::ec_please_login);

		SendWebMsg(WEB_SERVER_NET_Protocol::S2WEB::s2web_error, tagError);

		return;
	}

	WEB_SERVER_NET_Protocol::WEB2S_Del_Sensor	tagDelSensor;
	BYTE	*pSensorInfo = (BYTE*)pPack + sizeof(BYTE);
	tagDelSensor.ParseFromArray(pSensorInfo, uPackLen - sizeof(BYTE));

	SMysqlRequest	tagRequest	= {0};
	tagRequest.byOpt			= SENSOR_DB_DEL_SENSOR;
	tagRequest.uClientID		= m_uUniqueID;
	tagRequest.uClientIndex		= m_uIndex;
	tagRequest.byClientType		= WEB_CLIENT;

	IMysqlQuery	*pMysqlQuery	= g_ICenterServer.GetMysqlQuery();
	if (nullptr == pMysqlQuery)
		return;

	pMysqlQuery->PrepareProc("DeleteSensor");
	pMysqlQuery->AddParam(tagDelSensor.id());
	pMysqlQuery->EndPrepareProc(&tagRequest, sizeof(tagRequest));

	pMysqlQuery->CallProc();
}

void CWebClient::RecvUpdateSensor(const void *pPack, const unsigned int uPackLen)
{
	if (0 == m_uAccountID)
	{
		WEB_SERVER_NET_Protocol::S2WEB_ERROR	tagError;
		tagError.set_error_code(CommonDefine::ERROR_CODE::ec_please_login);

		SendWebMsg(WEB_SERVER_NET_Protocol::S2WEB::s2web_error, tagError);

		return;
	}

	WEB_SERVER_NET_Protocol::WEB2S_Update_Sensor_Data	tagSensorData;
	BYTE	*pSensorInfo = (BYTE*)pPack + sizeof(BYTE);
	tagSensorData.ParseFromArray(pSensorInfo, uPackLen - sizeof(BYTE));

	SMysqlRequest	tagRequest	= {0};
	tagRequest.byOpt			= SENSOR_DB_UPDATE_SENSOR;
	tagRequest.uClientID		= m_uUniqueID;
	tagRequest.uClientIndex		= m_uIndex;
	tagRequest.byClientType		= WEB_CLIENT;

	IMysqlQuery	*pMysqlQuery	= g_ICenterServer.GetMysqlQuery();
	if (nullptr == pMysqlQuery)
		return;

	pMysqlQuery->PrepareProc("UpdateSensor");
	pMysqlQuery->AddParam(tagSensorData.id());
	pMysqlQuery->AddParam(tagSensorData.type());
	pMysqlQuery->AddParam(tagSensorData.slope_id());
	pMysqlQuery->AddParam(tagSensorData.longitude());
	pMysqlQuery->AddParam(tagSensorData.latitude());
	pMysqlQuery->AddParam(tagSensorData.url().c_str());
	pMysqlQuery->AddParam(tagSensorData.description().c_str());
	pMysqlQuery->EndPrepareProc(&tagRequest, sizeof(tagRequest));

	pMysqlQuery->CallProc();
}

void CWebClient::RecvModifyPassword(const void *pPack, const unsigned int uPackLen)
{
	if (0 == m_uAccountID)
	{
		WEB_SERVER_NET_Protocol::S2WEB_ERROR	tagError;
		tagError.set_error_code(CommonDefine::ERROR_CODE::ec_please_login);

		SendWebMsg(WEB_SERVER_NET_Protocol::S2WEB::s2web_error, tagError);

		return;
	}

	WEB_SERVER_NET_Protocol::WEB2S_Modify_Password	tagModifyPassword;
	BYTE	*pSensorInfo = (BYTE*)pPack + sizeof(BYTE);
	tagModifyPassword.ParseFromArray(pSensorInfo, uPackLen - sizeof(BYTE));

	SMysqlRequest	tagRequest	= {0};
	tagRequest.byOpt			= SENSOR_DB_MODIFY_PASSWORD;
	tagRequest.uClientID		= m_uUniqueID;
	tagRequest.uClientIndex		= m_uIndex;
	tagRequest.byClientType		= WEB_CLIENT;

	IMysqlQuery	*pMysqlQuery	= g_ICenterServer.GetMysqlQuery();
	if (nullptr == pMysqlQuery)
		return;

	pMysqlQuery->PrepareProc("ModifyPassword");
	pMysqlQuery->AddParam(tagModifyPassword.account().c_str());
	pMysqlQuery->AddParam(tagModifyPassword.new_password().c_str());
	pMysqlQuery->EndPrepareProc(&tagRequest, sizeof(tagRequest));

	pMysqlQuery->CallProc();
}

void CWebClient::DBResopndLoginResult(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData)
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

	WEB_SERVER_NET_Protocol::S2Web_Login_Result	tagLoginResult;

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

	SendWebMsg(WEB_SERVER_NET_Protocol::S2WEB::s2web_login_result, tagLoginResult);
}

void CWebClient::DBResopndSlopeList(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData)
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

	WEB_SERVER_NET_Protocol::S2Web_Slope_List	tagSlopeList;

	for (auto uRow = 0; uRow < pResult1->GetRowCount(); ++uRow)
	{
		WEB_SERVER_NET_Protocol::S2Web_Slope_List::SlopeData	*pSlopeData = tagSlopeList.add_slope_list();
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
		pSlopeData->set_url(strUrl);
	}

	SendWebMsg(WEB_SERVER_NET_Protocol::S2WEB::s2web_slope_list, tagSlopeList);
}

void CWebClient::DBResopndSensorList(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData)
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
		g_pFileLog->WriteLog("CWebClient::DBResopndSensorList Return Row Count 0\n", __FILE__, __LINE__);
		return;
	}

	WEB_SERVER_NET_Protocol::S2Web_Sensor_List	tagSensorList;

	for (auto uRow = 0; uRow < pResult1->GetRowCount(); ++uRow)
	{
		WEB_SERVER_NET_Protocol::S2Web_Sensor_List::SensorData	*pSensor = tagSensorList.add_sensor_list();
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

	SendWebMsg(WEB_SERVER_NET_Protocol::S2WEB::s2web_sensor_list, tagSensorList);
}

void CWebClient::DBResopndSensorHistory(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData)
{
	UINT	uCol				= 0;
	UINT	uSensorID			= 0;
	double	dLongitude			= 0.0f;
	double	dLatitude			= 0.0f;
	int		nBeginTime			= 0;
	int		nEndTime			= 0;
	int		nDataTime			= 0;
	UINT	uInterval			= 0;
	double	dAvgValue1			= 0.0f;
	double	dAvgValue2			= 0.0f;
	double	dAvgValue3			= 0.0f;

	WORD	wSlopeID			= 0;
	double	dMinValue1			= 0.0f;
	double	dMinValue2			= 0.0f;
	double	dMinValue3			= 0.0f;
	double	dMaxValue1			= 0.0f;
	double	dMaxValue2			= 0.0f;
	double	dMaxValue3			= 0.0f;
	double	dMinOffsetValue1	= 0.0f;
	double	dMinOffsetValue2	= 0.0f;
	double	dMinOffsetValue3	= 0.0f;
	double	dMaxOffsetValue1	= 0.0f;
	double	dMaxOffsetValue2	= 0.0f;
	double	dMaxOffsetValue3	= 0.0f;

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

	WEB_SERVER_NET_Protocol::S2Web_Sensor_History	tagSensorHistory;

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
		WEB_SERVER_NET_Protocol::S2Web_Sensor_History::SensorData	*pSensor = tagSensorHistory.add_history_list();
		if (nullptr == pSensor)
			continue;

		uCol	= 0;

		pResult2->GetData(uRow, uCol++, dMinValue1);
		pResult2->GetData(uRow, uCol++, dMinValue2);
		pResult2->GetData(uRow, uCol++, dMinValue3);
		pResult2->GetData(uRow, uCol++, dMaxValue1);
		pResult2->GetData(uRow, uCol++, dMaxValue2);
		pResult2->GetData(uRow, uCol++, dMaxValue3);
		pResult2->GetData(uRow, uCol++, dMinOffsetValue1);
		pResult2->GetData(uRow, uCol++, dMinOffsetValue2);
		pResult2->GetData(uRow, uCol++, dMinOffsetValue3);
		pResult2->GetData(uRow, uCol++, dMaxOffsetValue1);
		pResult2->GetData(uRow, uCol++, dMaxOffsetValue2);
		pResult2->GetData(uRow, uCol++, dMaxOffsetValue3);

		pSensor->set_min_value1(dMinValue1);
		pSensor->set_min_value2(dMinValue2);
		pSensor->set_min_value3(dMinValue3);
		pSensor->set_max_value1(dMaxValue1);
		pSensor->set_max_value2(dMaxValue2);
		pSensor->set_max_value3(dMaxValue3);
		pSensor->set_min_offset_value1(dMinOffsetValue1);
		pSensor->set_min_offset_value2(dMinOffsetValue2);
		pSensor->set_min_offset_value3(dMinOffsetValue3);
		pSensor->set_max_offset_value1(dMaxOffsetValue1);
		pSensor->set_max_offset_value2(dMaxOffsetValue2);
		pSensor->set_max_offset_value3(dMaxOffsetValue3);
	}

	SendWebMsg(WEB_SERVER_NET_Protocol::S2WEB::s2web_sensor_history, tagSensorHistory);
}

void CWebClient::DBResopndAllList(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData)
{
}

void CWebClient::DBResopndAddSlopeResult(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData)
{
	UINT	uCol			= 0;
	BYTE	byResult		= 0;
	WORD	wSlopeID		= 0;
	BYTE	byType			= 0;
	char	strName[256]	= {0};
	BYTE	byState			= 0;
	double	dLongitude		= 0.0f;
	double	dLatitude		= 0.0f;
	char	strUrl[1024]	= {0};

	BYTE	byResultCount = pResultSet->GetResultCount();
	if (2 != byResultCount)
	{
		g_pFileLog->WriteLog("[%s][%d] Result Count[%hhu] Error\n", __FILE__, __LINE__, byResultCount);
		return;
	}

	IMysqlResult	*pResult1	= pResultSet->GetMysqlResult(0);
	IMysqlResult	*pResult2	= pResultSet->GetMysqlResult(1);

	if (nullptr == pResult1 || nullptr == pResult2)
		return;

	if (1 != pResult1->GetRowCount())
		return;

	pResult1->GetData(0, 0, byResult);
	if (0 != byResult)
	{
		WEB_SERVER_NET_Protocol::S2WEB_ERROR	tagError;
		tagError.set_error_code(byResult);

		SendWebMsg(WEB_SERVER_NET_Protocol::S2WEB::s2web_error, tagError);

		return;
	}

	WEB_SERVER_NET_Protocol::S2WEB_New_Slope	tagNewSlope;

	uCol	= 0;

	pResult2->GetData(0, uCol++, wSlopeID);
	pResult2->GetData(0, uCol++, byType);
	pResult2->GetData(0, uCol++, strName, sizeof(strName));
	pResult2->GetData(0, uCol++, dLongitude);
	pResult2->GetData(0, uCol++, dLatitude);
	pResult2->GetData(0, uCol++, strUrl, sizeof(strUrl));

	tagNewSlope.set_id(wSlopeID);
	tagNewSlope.set_type(byType);
	tagNewSlope.set_name(strName);
	tagNewSlope.set_state(0);
	tagNewSlope.set_longitude(dLongitude);
	tagNewSlope.set_latitude(dLatitude);
	tagNewSlope.set_url(strUrl);

	SendWebMsg(WEB_SERVER_NET_Protocol::S2WEB::s2web_new_slope, tagNewSlope);
}

void CWebClient::DBResopndDelSlopeResult(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData)
{
	UINT	uCol		= 0;
	BYTE	byResult	= 0;
	WORD	wSlopeID	= 0;

	BYTE	byResultCount = pResultSet->GetResultCount();
	if (2 != byResultCount)
	{
		g_pFileLog->WriteLog("[%s][%d] Result Count[%hhu] Error\n", __FILE__, __LINE__, byResultCount);
		return;
	}

	IMysqlResult	*pResult1	= pResultSet->GetMysqlResult(0);
	IMysqlResult	*pResult2	= pResultSet->GetMysqlResult(1);

	if (nullptr == pResult1 || nullptr == pResult2)
		return;

	if (1 != pResult1->GetRowCount())
		return;

	pResult1->GetData(0, 0, byResult);
	if (0 != byResult)
	{
		WEB_SERVER_NET_Protocol::S2WEB_ERROR	tagError;
		tagError.set_error_code(byResult);

		SendWebMsg(WEB_SERVER_NET_Protocol::S2WEB::s2web_error, tagError);

		return;
	}

	WEB_SERVER_NET_Protocol::S2WEB_Del_Slope	tagDelSlope;

	uCol	= 0;

	pResult2->GetData(0, uCol++, wSlopeID);

	tagDelSlope.set_id(wSlopeID);

	SendWebMsg(WEB_SERVER_NET_Protocol::S2WEB::s2web_del_slope, tagDelSlope);
}

void CWebClient::DBResopndUpdateSlopeResult(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData)
{
	UINT	uCol			= 0;
	BYTE	byResult		= 0;
	WORD	wSlopeID		= 0;
	BYTE	byType			= 0;
	char	strName[256]	= {0};
	BYTE	byState			= 0;
	double	dLongitude		= 0.0f;
	double	dLatitude		= 0.0f;
	char	strUrl[1024]	= {0};

	BYTE	byResultCount = pResultSet->GetResultCount();
	if (2 != byResultCount)
	{
		g_pFileLog->WriteLog("[%s][%d] Result Count[%hhu] Error\n", __FILE__, __LINE__, byResultCount);
		return;
	}

	IMysqlResult	*pResult1	= pResultSet->GetMysqlResult(0);
	IMysqlResult	*pResult2	= pResultSet->GetMysqlResult(1);

	if (nullptr == pResult1 || nullptr == pResult2)
		return;

	if (1 != pResult1->GetRowCount())
		return;

	pResult1->GetData(0, 0, byResult);
	if (0 != byResult)
	{
		WEB_SERVER_NET_Protocol::S2WEB_ERROR	tagError;
		tagError.set_error_code(byResult);

		SendWebMsg(WEB_SERVER_NET_Protocol::S2WEB::s2web_error, tagError);

		return;
	}

	WEB_SERVER_NET_Protocol::S2WEB_Update_Slope	tagUpdateSlope;

	uCol	= 0;

	pResult2->GetData(0, uCol++, wSlopeID);
	pResult2->GetData(0, uCol++, byType);
	pResult2->GetData(0, uCol++, strName, sizeof(strName));
	pResult2->GetData(0, uCol++, dLongitude);
	pResult2->GetData(0, uCol++, dLatitude);
	pResult2->GetData(0, uCol++, strUrl, sizeof(strUrl));

	tagUpdateSlope.set_id(wSlopeID);
	tagUpdateSlope.set_type(byType);
	tagUpdateSlope.set_name(strName);
	tagUpdateSlope.set_state(0);
	tagUpdateSlope.set_longitude(dLongitude);
	tagUpdateSlope.set_latitude(dLatitude);
	tagUpdateSlope.set_url(strUrl);

	SendWebMsg(WEB_SERVER_NET_Protocol::S2WEB::s2web_update_slope, tagUpdateSlope);
}

void CWebClient::DBResopndAddSensorResult(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData)
{
	UINT	uCol					= 0;
	BYTE	byResult				= 0;
	UINT	uSensorID				= 0;
	BYTE	byType					= 0;
	BYTE	byState					= 0;
	WORD	wSlopeID				= 0;
	double	dLongitude				= 0.0f;
	double	dLatitude				= 0.0f;
	char	strUrl[1024]			= {0};
	char	strDescription[1024]	= {0};

	BYTE	byResultCount = pResultSet->GetResultCount();
	if (2 != byResultCount)
	{
		g_pFileLog->WriteLog("[%s][%d] Result Count[%hhu] Error\n", __FILE__, __LINE__, byResultCount);
		return;
	}

	IMysqlResult	*pResult1	= pResultSet->GetMysqlResult(0);
	IMysqlResult	*pResult2	= pResultSet->GetMysqlResult(1);

	if (nullptr == pResult1 || nullptr == pResult2)
		return;

	if (1 != pResult1->GetRowCount())
		return;

	pResult1->GetData(0, 0, byResult);
	if (0 != byResult)
	{
		WEB_SERVER_NET_Protocol::S2WEB_ERROR	tagError;
		tagError.set_error_code(byResult);

		SendWebMsg(WEB_SERVER_NET_Protocol::S2WEB::s2web_error, tagError);

		return;
	}

	WEB_SERVER_NET_Protocol::S2WEB_Add_Sensor	tagNewSensor;

	uCol	= 0;

	pResult2->GetData(0, uCol++, uSensorID);
	pResult2->GetData(0, uCol++, byType);
	pResult2->GetData(0, uCol++, wSlopeID);
	pResult2->GetData(0, uCol++, dLongitude);
	pResult2->GetData(0, uCol++, dLatitude);
	pResult2->GetData(0, uCol++, strUrl, sizeof(strUrl));
	pResult2->GetData(0, uCol++, strDescription, sizeof(strDescription));

	tagNewSensor.set_id(uSensorID);
	tagNewSensor.set_type(byType);
	tagNewSensor.set_slope_id(wSlopeID);
	tagNewSensor.set_state(0);
	tagNewSensor.set_longitude(dLongitude);
	tagNewSensor.set_latitude(dLatitude);
	tagNewSensor.set_url(strUrl);
	tagNewSensor.set_description(strDescription);

	SendWebMsg(WEB_SERVER_NET_Protocol::S2WEB::s2web_add_sensor, tagNewSensor);
}

void CWebClient::DBResopndDelSensorResult(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData)
{
	UINT	uCol		= 0;
	BYTE	byResult	= 0;
	UINT	uSensorID	= 0;

	BYTE	byResultCount = pResultSet->GetResultCount();
	if (2 != byResultCount)
	{
		g_pFileLog->WriteLog("[%s][%d] Result Count[%hhu] Error\n", __FILE__, __LINE__, byResultCount);
		return;
	}

	IMysqlResult	*pResult1	= pResultSet->GetMysqlResult(0);
	IMysqlResult	*pResult2	= pResultSet->GetMysqlResult(1);

	if (nullptr == pResult1 || nullptr == pResult2)
		return;

	if (1 != pResult1->GetRowCount())
		return;

	pResult1->GetData(0, 0, byResult);
	if (0 != byResult)
	{
		WEB_SERVER_NET_Protocol::S2WEB_ERROR	tagError;
		tagError.set_error_code(byResult);

		SendWebMsg(WEB_SERVER_NET_Protocol::S2WEB::s2web_error, tagError);

		return;
	}

	WEB_SERVER_NET_Protocol::S2WEB_Del_Sensor	tagDelSensor;

	uCol	= 0;

	pResult2->GetData(0, uCol++, uSensorID);

	tagDelSensor.set_id(uSensorID);

	SendWebMsg(WEB_SERVER_NET_Protocol::S2WEB::s2web_del_sensor, tagDelSensor);
}

void CWebClient::DBResopndUpdateSensorResult(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData)
{
	UINT	uCol					= 0;
	BYTE	byResult				= 0;
	UINT	uSensorID				= 0;
	BYTE	byType					= 0;
	BYTE	byState					= 0;
	WORD	wSlopeID				= 0;
	double	dLongitude				= 0.0f;
	double	dLatitude				= 0.0f;
	char	strUrl[1024]			= {0};
	char	strDescription[1024]	= {0};

	BYTE	byResultCount = pResultSet->GetResultCount();
	if (2 != byResultCount)
	{
		g_pFileLog->WriteLog("[%s][%d] Result Count[%hhu] Error\n", __FILE__, __LINE__, byResultCount);
		return;
	}

	IMysqlResult	*pResult1	= pResultSet->GetMysqlResult(0);
	IMysqlResult	*pResult2	= pResultSet->GetMysqlResult(1);

	if (nullptr == pResult1 || nullptr == pResult2)
		return;

	if (1 != pResult1->GetRowCount())
		return;

	pResult1->GetData(0, 0, byResult);
	if (0 != byResult)
	{
		WEB_SERVER_NET_Protocol::S2WEB_ERROR	tagError;
		tagError.set_error_code(byResult);

		SendWebMsg(WEB_SERVER_NET_Protocol::S2WEB::s2web_error, tagError);

		return;
	}

	WEB_SERVER_NET_Protocol::S2WEB_Update_Sensor	tagUpdateSensor;

	uCol	= 0;

	pResult2->GetData(0, uCol++, uSensorID);
	pResult2->GetData(0, uCol++, byType);
	pResult2->GetData(0, uCol++, wSlopeID);
	pResult2->GetData(0, uCol++, dLongitude);
	pResult2->GetData(0, uCol++, dLatitude);
	pResult2->GetData(0, uCol++, strUrl, sizeof(strUrl));
	pResult2->GetData(0, uCol++, strDescription, sizeof(strDescription));

	tagUpdateSensor.set_id(uSensorID);
	tagUpdateSensor.set_type(byType);
	tagUpdateSensor.set_slope_id(wSlopeID);
	tagUpdateSensor.set_state(0);
	tagUpdateSensor.set_longitude(dLongitude);
	tagUpdateSensor.set_latitude(dLatitude);
	tagUpdateSensor.set_url(strUrl);
	tagUpdateSensor.set_description(strDescription);

	SendWebMsg(WEB_SERVER_NET_Protocol::S2WEB::s2web_update_sensor, tagUpdateSensor);
}

void CWebClient::DBResopndModifyPasswordResult(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData)
{
	BYTE	byResultCount = pResultSet->GetResultCount();
	if (1 != byResultCount)
	{
		g_pFileLog->WriteLog("[%s][%d] Result Count[%hhu] Error\n", __FILE__, __LINE__, byResultCount);
		return;
	}

	IMysqlResult	*pResult1	= pResultSet->GetMysqlResult(0);

	if (nullptr == pResult1)
		return;

	if (1 != pResult1->GetRowCount())
		return;

	BYTE	byResult	= 0;

	pResult1->GetData(0, 0, byResult);

	WEB_SERVER_NET_Protocol::S2WEB_Modify_Password_Result	tagModifyPassword;
	tagModifyPassword.set_result(byResult);


	SendWebMsg(WEB_SERVER_NET_Protocol::S2WEB::s2web_modify_password_result, tagModifyPassword);
}

void CWebClient::SendWebMsg(const BYTE byProtocol, google::protobuf::Message &tagMsg)
{
	char	strBuffer[0xffff]	= {0};
	if (sizeof(BYTE)+tagMsg.ByteSize() > sizeof(strBuffer))
		return;

	*(BYTE*)strBuffer	= byProtocol;
	tagMsg.SerializeToArray(strBuffer + sizeof(BYTE), tagMsg.ByteSize());

	m_pClientConn->PutPack(strBuffer, sizeof(BYTE)+tagMsg.ByteSize());

	/*
	if (WEB_SERVER_NET_Protocol::S2WEB::s2web_slope_list == byProtocol)
	{
		FILE	*pFile = fopen("s2web_slope_list.txt", "a+");
		if (nullptr == pFile)
		{
			g_pFileLog->WriteLog("Open s2web_slope_list.txt Failed\n", __FILE__, __LINE__);
			return;
		}

		fwrite(strBuffer, 1, sizeof(BYTE)+tagMsg.ByteSize(), pFile);
		fprintf(pFile, "\n");

		fclose(pFile);
	}
	*/
}
