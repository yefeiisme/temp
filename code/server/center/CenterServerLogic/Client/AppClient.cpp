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
	&CAppClient::RecvAddSlope,
	&CAppClient::RecvDelSlope,
	&CAppClient::RecvUpdateSlope,
	&CAppClient::RecvAddSensor,

	&CAppClient::RecvDelSensor,
	&CAppClient::RecvUpdateSensor,
	&CAppClient::RecvModifyPassword,
	&CAppClient::RecvFindSlope,
	&CAppClient::RecvFindSensor,

	&CAppClient::RecvLoadAuthority,
};

CAppClient::pfnDBRespondFunc CAppClient::m_pfnDBRespondFunc[SENSOR_DB_OPT_MAX]
{
	&CAppClient::DBResopndLoginResult,
	&CAppClient::DBResopndSlopeList,
	&CAppClient::DBResopndSensorList,
	&CAppClient::DBResopndSensorHistory,
	&CAppClient::DBResopndAllList,

	&CAppClient::DBResopndAddSlopeResult,
	&CAppClient::DBResopndDelSlopeResult,
	&CAppClient::DBResopndUpdateSlopeResult,
	&CAppClient::DBResopndAddSensorResult,
	&CAppClient::DBResopndDelSensorResult,

	&CAppClient::DBResopndUpdateSensorResult,
	&CAppClient::DBResopndModifyPasswordResult,
	&CAppClient::DBResopndFindSlopeResult,
	&CAppClient::DBResopndFindSensorResult,
	&CAppClient::DBResopndLoadAuthor,
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

void CAppClient::RecvAddSlope(const void *pPack, const unsigned int uPackLen)
{
	if (0 == m_uAccountID)
	{
		APP_SERVER_NET_Protocol::S2APP_ERROR	tagError;
		tagError.set_error_code(CommonDefine::ERROR_CODE::ec_please_login);

		SendAppMsg(APP_SERVER_NET_Protocol::S2APP::s2app_error, tagError);

		return;
	}

	APP_SERVER_NET_Protocol::APP2S_Add_Slope	tagAddSlope;
	BYTE	*pSlopeInfo = (BYTE*)pPack + sizeof(BYTE);
	tagAddSlope.ParseFromArray(pSlopeInfo, uPackLen - sizeof(BYTE));

	SMysqlRequest	tagRequest	= {0};
	tagRequest.byOpt			= SENSOR_DB_ADD_SLOPE;
	tagRequest.uClientID		= m_uUniqueID;
	tagRequest.uClientIndex		= m_uIndex;
	tagRequest.byClientType		= APP_CLIENT;

	IMysqlQuery	*pMysqlQuery	= g_ICenterServer.GetMysqlQuery();
	if (nullptr == pMysqlQuery)
		return;

	pMysqlQuery->PrepareProc("AddSlope");
	pMysqlQuery->AddParam(tagAddSlope.scene_id());
	pMysqlQuery->AddParam(tagAddSlope.type());
	pMysqlQuery->AddParam(tagAddSlope.name().c_str());
	pMysqlQuery->AddParam(m_uAccountID);
	pMysqlQuery->AddParam(tagAddSlope.url().c_str());
	pMysqlQuery->EndPrepareProc(&tagRequest, sizeof(tagRequest));

	pMysqlQuery->CallProc();
}

void CAppClient::RecvDelSlope(const void *pPack, const unsigned int uPackLen)
{
	if (0 == m_uAccountID)
	{
		APP_SERVER_NET_Protocol::S2APP_ERROR	tagError;
		tagError.set_error_code(CommonDefine::ERROR_CODE::ec_please_login);

		SendAppMsg(APP_SERVER_NET_Protocol::S2APP::s2app_error, tagError);

		return;
	}

	APP_SERVER_NET_Protocol::APP2S_Del_Slope	tagDelSlope;
	BYTE	*pSlopeInfo = (BYTE*)pPack + sizeof(BYTE);
	tagDelSlope.ParseFromArray(pSlopeInfo, uPackLen - sizeof(BYTE));

	SMysqlRequest	tagRequest	= {0};
	tagRequest.byOpt			= SENSOR_DB_DEL_SLOPE;
	tagRequest.uClientID		= m_uUniqueID;
	tagRequest.uClientIndex		= m_uIndex;
	tagRequest.byClientType		= APP_CLIENT;

	IMysqlQuery	*pMysqlQuery	= g_ICenterServer.GetMysqlQuery();
	if (nullptr == pMysqlQuery)
		return;

	pMysqlQuery->PrepareProc("DeleteSlope");
	pMysqlQuery->AddParam(tagDelSlope.id());
	pMysqlQuery->EndPrepareProc(&tagRequest, sizeof(tagRequest));

	pMysqlQuery->CallProc();
}

void CAppClient::RecvUpdateSlope(const void *pPack, const unsigned int uPackLen)
{
	if (0 == m_uAccountID)
	{
		APP_SERVER_NET_Protocol::S2APP_ERROR	tagError;
		tagError.set_error_code(CommonDefine::ERROR_CODE::ec_please_login);

		SendAppMsg(APP_SERVER_NET_Protocol::S2APP::s2app_error, tagError);

		return;
	}

	APP_SERVER_NET_Protocol::APP2S_Update_Slope_Data	tagSlopeData;
	BYTE	*pSlopeInfo = (BYTE*)pPack + sizeof(BYTE);
	tagSlopeData.ParseFromArray(pSlopeInfo, uPackLen - sizeof(BYTE));

	SMysqlRequest	tagRequest	= {0};
	tagRequest.byOpt			= SENSOR_DB_UPDATE_SLOPE;
	tagRequest.uClientID		= m_uUniqueID;
	tagRequest.uClientIndex		= m_uIndex;
	tagRequest.byClientType		= APP_CLIENT;

	IMysqlQuery	*pMysqlQuery	= g_ICenterServer.GetMysqlQuery();
	if (nullptr == pMysqlQuery)
		return;

	pMysqlQuery->PrepareProc("UpdateSlope");
	pMysqlQuery->AddParam(tagSlopeData.id());
	pMysqlQuery->AddParam(tagSlopeData.scene_id());
	pMysqlQuery->AddParam(tagSlopeData.type());
	pMysqlQuery->AddParam(tagSlopeData.name().c_str());
	pMysqlQuery->AddParam(tagSlopeData.longitude());
	pMysqlQuery->AddParam(tagSlopeData.latitude());
	pMysqlQuery->AddParam(m_uAccountID);
	pMysqlQuery->AddParam(tagSlopeData.url().c_str());
	pMysqlQuery->EndPrepareProc(&tagRequest, sizeof(tagRequest));

	pMysqlQuery->CallProc();
}

void CAppClient::RecvAddSensor(const void *pPack, const unsigned int uPackLen)
{
	if (0 == m_uAccountID)
	{
		APP_SERVER_NET_Protocol::S2APP_ERROR	tagError;
		tagError.set_error_code(CommonDefine::ERROR_CODE::ec_please_login);

		SendAppMsg(APP_SERVER_NET_Protocol::S2APP::s2app_error, tagError);

		return;
	}

	APP_SERVER_NET_Protocol::APP2S_Add_Sensor	tagAddSensor;
	BYTE	*pSensorInfo = (BYTE*)pPack + sizeof(BYTE);
	tagAddSensor.ParseFromArray(pSensorInfo, uPackLen - sizeof(BYTE));

	SMysqlRequest	tagRequest	= {0};
	tagRequest.byOpt			= SENSOR_DB_ADD_SENSOR;
	tagRequest.uClientID		= m_uUniqueID;
	tagRequest.uClientIndex		= m_uIndex;
	tagRequest.byClientType		= APP_CLIENT;

	IMysqlQuery	*pMysqlQuery	= g_ICenterServer.GetMysqlQuery();
	if (nullptr == pMysqlQuery)
		return;

	pMysqlQuery->PrepareProc("AddSensor");
	pMysqlQuery->AddParam(tagAddSensor.scene_id());
	pMysqlQuery->AddParam(tagAddSensor.type());
	pMysqlQuery->AddParam(tagAddSensor.slope_id());
	pMysqlQuery->AddParam(tagAddSensor.longitude());
	pMysqlQuery->AddParam(tagAddSensor.latitude());
	pMysqlQuery->AddParam(tagAddSensor.url().c_str());
	pMysqlQuery->AddParam(tagAddSensor.description().c_str());
	pMysqlQuery->EndPrepareProc(&tagRequest, sizeof(tagRequest));

	pMysqlQuery->CallProc();
}

void CAppClient::RecvDelSensor(const void *pPack, const unsigned int uPackLen)
{
	if (0 == m_uAccountID)
	{
		APP_SERVER_NET_Protocol::S2APP_ERROR	tagError;
		tagError.set_error_code(CommonDefine::ERROR_CODE::ec_please_login);

		SendAppMsg(APP_SERVER_NET_Protocol::S2APP::s2app_error, tagError);

		return;
	}

	APP_SERVER_NET_Protocol::APP2S_Del_Sensor	tagDelSensor;
	BYTE	*pSensorInfo = (BYTE*)pPack + sizeof(BYTE);
	tagDelSensor.ParseFromArray(pSensorInfo, uPackLen - sizeof(BYTE));

	SMysqlRequest	tagRequest	= {0};
	tagRequest.byOpt			= SENSOR_DB_DEL_SENSOR;
	tagRequest.uClientID		= m_uUniqueID;
	tagRequest.uClientIndex		= m_uIndex;
	tagRequest.byClientType		= APP_CLIENT;

	IMysqlQuery	*pMysqlQuery	= g_ICenterServer.GetMysqlQuery();
	if (nullptr == pMysqlQuery)
		return;

	pMysqlQuery->PrepareProc("DeleteSensor");
	pMysqlQuery->AddParam(tagDelSensor.id());
	pMysqlQuery->EndPrepareProc(&tagRequest, sizeof(tagRequest));

	pMysqlQuery->CallProc();
}

void CAppClient::RecvUpdateSensor(const void *pPack, const unsigned int uPackLen)
{
	if (0 == m_uAccountID)
	{
		APP_SERVER_NET_Protocol::S2APP_ERROR	tagError;
		tagError.set_error_code(CommonDefine::ERROR_CODE::ec_please_login);

		SendAppMsg(APP_SERVER_NET_Protocol::S2APP::s2app_error, tagError);

		return;
	}

	APP_SERVER_NET_Protocol::APP2S_Update_Sensor_Data	tagSensorData;
	BYTE	*pSensorInfo = (BYTE*)pPack + sizeof(BYTE);
	tagSensorData.ParseFromArray(pSensorInfo, uPackLen - sizeof(BYTE));

	SMysqlRequest	tagRequest	= {0};
	tagRequest.byOpt			= SENSOR_DB_UPDATE_SENSOR;
	tagRequest.uClientID		= m_uUniqueID;
	tagRequest.uClientIndex		= m_uIndex;
	tagRequest.byClientType		= APP_CLIENT;

	IMysqlQuery	*pMysqlQuery	= g_ICenterServer.GetMysqlQuery();
	if (nullptr == pMysqlQuery)
		return;

	pMysqlQuery->PrepareProc("UpdateSensor");
	pMysqlQuery->AddParam(tagSensorData.id());
	pMysqlQuery->AddParam(tagSensorData.scene_id());
	pMysqlQuery->AddParam(tagSensorData.type());
	pMysqlQuery->AddParam(tagSensorData.slope_id());
	pMysqlQuery->AddParam(tagSensorData.longitude());
	pMysqlQuery->AddParam(tagSensorData.latitude());
	pMysqlQuery->AddParam(tagSensorData.url().c_str());
	pMysqlQuery->AddParam(tagSensorData.description().c_str());
	pMysqlQuery->EndPrepareProc(&tagRequest, sizeof(tagRequest));

	pMysqlQuery->CallProc();
}

void CAppClient::RecvModifyPassword(const void *pPack, const unsigned int uPackLen)
{
	if (0 == m_uAccountID)
	{
		APP_SERVER_NET_Protocol::S2APP_ERROR	tagError;
		tagError.set_error_code(CommonDefine::ERROR_CODE::ec_please_login);

		SendAppMsg(APP_SERVER_NET_Protocol::S2APP::s2app_error, tagError);

		return;
	}

	APP_SERVER_NET_Protocol::APP2S_Modify_Password	tagModifyPassword;
	BYTE	*pSensorInfo = (BYTE*)pPack + sizeof(BYTE);
	tagModifyPassword.ParseFromArray(pSensorInfo, uPackLen - sizeof(BYTE));

	SMysqlRequest	tagRequest	={ 0 };
	tagRequest.byOpt			= SENSOR_DB_MODIFY_PASSWORD;
	tagRequest.uClientID		= m_uUniqueID;
	tagRequest.uClientIndex		= m_uIndex;
	tagRequest.byClientType		= APP_CLIENT;

	IMysqlQuery	*pMysqlQuery	= g_ICenterServer.GetMysqlQuery();
	if (nullptr == pMysqlQuery)
		return;

	pMysqlQuery->PrepareProc("ModifyPassword");
	pMysqlQuery->AddParam(tagModifyPassword.account().c_str());
	pMysqlQuery->AddParam(tagModifyPassword.new_password().c_str());
	pMysqlQuery->EndPrepareProc(&tagRequest, sizeof(tagRequest));

	pMysqlQuery->CallProc();
}

void CAppClient::RecvFindSlope(const void *pPack, const unsigned int uPackLen)
{
	if (0 == m_uAccountID)
	{
		APP_SERVER_NET_Protocol::S2APP_ERROR	tagError;
		tagError.set_error_code(CommonDefine::ERROR_CODE::ec_please_login);

		SendAppMsg(APP_SERVER_NET_Protocol::S2APP::s2app_error, tagError);

		return;
	}

	APP_SERVER_NET_Protocol::APP2S_Find_Slope	tagFindSlope;
	BYTE	*pSensorInfo = (BYTE*)pPack + sizeof(BYTE);
	tagFindSlope.ParseFromArray(pSensorInfo, uPackLen - sizeof(BYTE));

	SMysqlRequest	tagRequest	= {0};
	tagRequest.byOpt			= SENSOR_DB_FIND_SLOPE;
	tagRequest.uClientID		= m_uUniqueID;
	tagRequest.uClientIndex		= m_uIndex;
	tagRequest.byClientType		= APP_CLIENT;

	IMysqlQuery	*pMysqlQuery	= g_ICenterServer.GetMysqlQuery();
	if (nullptr == pMysqlQuery)
		return;

	pMysqlQuery->PrepareProc("FindSlope");
	pMysqlQuery->AddParam(tagFindSlope.slope_id());
	pMysqlQuery->AddParam(tagFindSlope.slope_name().c_str());
	pMysqlQuery->EndPrepareProc(&tagRequest, sizeof(tagRequest));

	pMysqlQuery->CallProc();
}

void CAppClient::RecvFindSensor(const void *pPack, const unsigned int uPackLen)
{
	if (0 == m_uAccountID)
	{
		APP_SERVER_NET_Protocol::S2APP_ERROR	tagError;
		tagError.set_error_code(CommonDefine::ERROR_CODE::ec_please_login);

		SendAppMsg(APP_SERVER_NET_Protocol::S2APP::s2app_error, tagError);

		return;
	}

	APP_SERVER_NET_Protocol::APP2S_Find_Sensor	tagFindSensor;
	BYTE	*pSensorInfo = (BYTE*)pPack + sizeof(BYTE);
	tagFindSensor.ParseFromArray(pSensorInfo, uPackLen - sizeof(BYTE));

	SMysqlRequest	tagRequest	= {0};
	tagRequest.byOpt			= SENSOR_DB_FIND_SENSOR;
	tagRequest.uClientID		= m_uUniqueID;
	tagRequest.uClientIndex		= m_uIndex;
	tagRequest.byClientType		= APP_CLIENT;

	IMysqlQuery	*pMysqlQuery	= g_ICenterServer.GetMysqlQuery();
	if (nullptr == pMysqlQuery)
		return;

	pMysqlQuery->PrepareProc("FindSensor");
	pMysqlQuery->AddParam(tagFindSensor.slope_id());
	pMysqlQuery->AddParam(tagFindSensor.slope_name().c_str());
	pMysqlQuery->AddParam(tagFindSensor.sensor_id());
	pMysqlQuery->AddParam(tagFindSensor.sensor_type());
	pMysqlQuery->EndPrepareProc(&tagRequest, sizeof(tagRequest));

	pMysqlQuery->CallProc();
}

void CAppClient::RecvLoadAuthority(const void *pPack, const unsigned int uPackLen)
{
	if (0 == m_uAccountID)
	{
		APP_SERVER_NET_Protocol::S2APP_ERROR	tagError;
		tagError.set_error_code(CommonDefine::ERROR_CODE::ec_please_login);

		SendAppMsg(APP_SERVER_NET_Protocol::S2APP::s2app_error, tagError);

		return;
	}

	APP_SERVER_NET_Protocol::APP2S_Load_Authority	tagLoadAuthority;
	BYTE	*pRequest = (BYTE*)pPack + sizeof(BYTE);
	tagLoadAuthority.ParseFromArray(pRequest, uPackLen - sizeof(BYTE));

	SMysqlRequest	tagRequest = {0};
	tagRequest.byOpt		= SENSOR_DB_LOAD_AUTHORITY;
	tagRequest.uClientID	= m_uUniqueID;
	tagRequest.uClientIndex = m_uIndex;
	tagRequest.byClientType = APP_CLIENT;

	IMysqlQuery	*pMysqlQuery = g_ICenterServer.GetMysqlQuery();
	if (nullptr == pMysqlQuery)
		return;

	if (tagLoadAuthority.authority_id())
	{
		pMysqlQuery->PrepareProc("LoadAuthorityByID");
		pMysqlQuery->AddParam(m_uAccountID);
		pMysqlQuery->AddParam(tagLoadAuthority.authority_id());
		pMysqlQuery->EndPrepareProc(&tagRequest, sizeof(tagRequest));
	}
	else
	{
		pMysqlQuery->PrepareProc("LoadAllAuthority");
		pMysqlQuery->AddParam(m_uAccountID);
		pMysqlQuery->EndPrepareProc(&tagRequest, sizeof(tagRequest));
	}

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
	WORD	wSceneID			= 0;
	BYTE	bySlopeType			= 0;
	char	strSlopeName[64]	= {0};
	double	dLongitude			= 0.0f;
	double	dLatitude			= 0.0f;
	BYTE	byState				= 0;
	char	strUrl[0xffff]		= {0};
	char	strDesc[0xffff]		= {0};

	BYTE	byResultCount = pResultSet->GetResultCount();
	if (1 != byResultCount)
	{
		g_pFileLog->WriteLog("[%s][%d] Result Count[%hhu] Error\n", __FILE__, __LINE__, byResultCount);
		return;
	}

	IMysqlResult	*pResult1 = pResultSet->GetMysqlResult(0);

	if (nullptr == pResult1)
		return;

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
		pResult1->GetData(uRow, uCol++, wSceneID);
		pResult1->GetData(uRow, uCol++, bySlopeType);
		pResult1->GetData(uRow, uCol++, strSlopeName, sizeof(strSlopeName));
		pResult1->GetData(uRow, uCol++, dLongitude);
		pResult1->GetData(uRow, uCol++, dLatitude);
		pResult1->GetData(uRow, uCol++, byState);
		pResult1->GetData(uRow, uCol++, strUrl, sizeof(strUrl));
		pResult1->GetData(uRow, uCol++, strDesc, sizeof(strDesc));

		pSlopeData->set_id(wSlopeID);
		pSlopeData->set_scene_id(wSlopeID);
		pSlopeData->set_type(bySlopeType);
		pSlopeData->set_name(strSlopeName);
		pSlopeData->set_longitude(dLongitude);
		pSlopeData->set_latitude(dLatitude);
		pSlopeData->set_state(byState);
		pSlopeData->set_desc(strDesc);
	}

	SendAppMsg(APP_SERVER_NET_Protocol::S2APP::s2app_slope_list, tagSlopeList);
}

void CAppClient::DBResopndSensorList(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData)
{
	UINT	uCol			= 0;
	UINT	uSensorID		= 0;
	WORD	wSceneID		= 0;
	BYTE	bySensorType	= 0;
	double	dCurValue1		= 0.0;
	double	dCurValue2		= 0.0;
	double	dCurValue3		= 0.0;
	double	dCurValue4		= 0.0;
	double	dAvgValue1		= 0.0;
	double	dAvgValue2		= 0.0;
	double	dAvgValue3		= 0.0;
	double	dOffsetValue1	= 0.0;
	double	dOffsetValue2	= 0.0;
	double	dOffsetValue3	= 0.0;
	BYTE	byState			= 0;
	WORD	wSlopeID		= 0;
	double	dLongitude		= 0.0;
	double	dLatitude		= 0.0;
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
		return;

	APP_SERVER_NET_Protocol::S2App_Sensor_List	tagSensorList;

	for (auto uRow = 0; uRow < pResult1->GetRowCount(); ++uRow)
	{
		APP_SERVER_NET_Protocol::S2App_Sensor_List::SensorData	*pSensor = tagSensorList.add_sensor_list();
		if (nullptr == pSensor)
			continue;

		uCol	= 0;

		pResult1->GetData(uRow, uCol++, uSensorID);
		pResult1->GetData(uRow, uCol++, wSceneID);
		pResult1->GetData(uRow, uCol++, bySensorType);
		pResult1->GetData(uRow, uCol++, dCurValue1);
		pResult1->GetData(uRow, uCol++, dCurValue2);
		pResult1->GetData(uRow, uCol++, dCurValue3);
		pResult1->GetData(uRow, uCol++, dCurValue4);
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
		pSensor->set_scene_id(wSceneID);
		pSensor->set_type(bySensorType);
		pSensor->set_cur_value1(dCurValue1);
		pSensor->set_cur_value2(dCurValue2);
		pSensor->set_cur_value3(dCurValue3);
		pSensor->set_cur_value4(dCurValue4);
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
	UINT	uCol				= 0;
	UINT	uSensorID			= 0;
	WORD	wSceneID			= 0;
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
	tagSensorHistory.set_scene_id(wSceneID);

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

void CAppClient::DBResopndAddSlopeResult(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData)
{
	UINT	uCol			= 0;
	BYTE	byResult		= 0;
	WORD	wSlopeID		= 0;
	WORD	wSceneID		= 0;
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

	IMysqlResult	*pResult1 = pResultSet->GetMysqlResult(0);
	IMysqlResult	*pResult2 = pResultSet->GetMysqlResult(1);

	if (nullptr == pResult1 || nullptr == pResult2)
		return;

	if (1 != pResult1->GetRowCount())
		return;

	pResult1->GetData(0, 0, byResult);
	if (0 != byResult)
	{
		APP_SERVER_NET_Protocol::S2APP_ERROR	tagError;
		tagError.set_error_code(byResult);

		SendAppMsg(APP_SERVER_NET_Protocol::S2APP::s2app_error, tagError);

		return;
	}

	APP_SERVER_NET_Protocol::S2APP_New_Slope	tagNewSlope;

	uCol = 0;

	pResult2->GetData(0, uCol++, wSlopeID);
	pResult2->GetData(0, uCol++, wSceneID);
	pResult2->GetData(0, uCol++, byType);
	pResult2->GetData(0, uCol++, strName, sizeof(strName));
	pResult2->GetData(0, uCol++, dLongitude);
	pResult2->GetData(0, uCol++, dLatitude);
	pResult2->GetData(0, uCol++, strUrl, sizeof(strUrl));

	tagNewSlope.set_id(wSlopeID);
	tagNewSlope.set_scene_id(wSceneID);
	tagNewSlope.set_type(byType);
	tagNewSlope.set_name(strName);
	tagNewSlope.set_state(0);
	tagNewSlope.set_longitude(dLongitude);
	tagNewSlope.set_latitude(dLatitude);
	tagNewSlope.set_url(strUrl);

	SendAppMsg(APP_SERVER_NET_Protocol::S2APP::s2app_new_slope, tagNewSlope);
}

void CAppClient::DBResopndDelSlopeResult(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData)
{
	UINT	uCol			= 0;
	BYTE	byResult		= 0;
	BYTE	byResultCount	= pResultSet->GetResultCount();
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

	pResult1->GetData(0, 0, byResult);

	APP_SERVER_NET_Protocol::S2APP_Del_Slope	tagDelSlope;

	tagDelSlope.set_result(byResult);

	SendAppMsg(APP_SERVER_NET_Protocol::S2APP::s2app_del_slope, tagDelSlope);
}

void CAppClient::DBResopndUpdateSlopeResult(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData)
{
	UINT	uCol			= 0;
	BYTE	byResult		= 0;
	WORD	wSlopeID		= 0;
	WORD	wSceneID		= 0;
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
		APP_SERVER_NET_Protocol::S2APP_ERROR	tagError;
		tagError.set_error_code(byResult);

		SendAppMsg(APP_SERVER_NET_Protocol::S2APP::s2app_error, tagError);

		return;
	}

	APP_SERVER_NET_Protocol::S2APP_Update_Slope	tagUpdateSlope;

	uCol	= 0;

	pResult2->GetData(0, uCol++, wSlopeID);
	pResult2->GetData(0, uCol++, wSceneID);
	pResult2->GetData(0, uCol++, byType);
	pResult2->GetData(0, uCol++, strName, sizeof(strName));
	pResult2->GetData(0, uCol++, dLongitude);
	pResult2->GetData(0, uCol++, dLatitude);
	pResult2->GetData(0, uCol++, strUrl, sizeof(strUrl));

	tagUpdateSlope.set_id(wSlopeID);
	tagUpdateSlope.set_scene_id(wSceneID);
	tagUpdateSlope.set_type(byType);
	tagUpdateSlope.set_name(strName);
	tagUpdateSlope.set_state(0);
	tagUpdateSlope.set_longitude(dLongitude);
	tagUpdateSlope.set_latitude(dLatitude);
	tagUpdateSlope.set_url(strUrl);

	SendAppMsg(APP_SERVER_NET_Protocol::S2APP::s2app_update_slope, tagUpdateSlope);
}

void CAppClient::DBResopndAddSensorResult(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData)
{
	UINT	uCol					= 0;
	BYTE	byResult				= 0;
	UINT	uSensorID				= 0;
	WORD	wSceneID				= 0;
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
		APP_SERVER_NET_Protocol::S2APP_ERROR	tagError;
		tagError.set_error_code(byResult);

		SendAppMsg(APP_SERVER_NET_Protocol::S2APP::s2app_error, tagError);

		return;
	}

	APP_SERVER_NET_Protocol::S2APP_Add_Sensor	tagNewSensor;

	uCol	= 0;

	pResult2->GetData(0, uCol++, uSensorID);
	pResult2->GetData(0, uCol++, wSceneID);
	pResult2->GetData(0, uCol++, byType);
	pResult2->GetData(0, uCol++, wSlopeID);
	pResult2->GetData(0, uCol++, dLongitude);
	pResult2->GetData(0, uCol++, dLatitude);
	pResult2->GetData(0, uCol++, strUrl, sizeof(strUrl));
	pResult2->GetData(0, uCol++, strDescription, sizeof(strDescription));

	tagNewSensor.set_id(uSensorID);
	tagNewSensor.set_scene_id(wSceneID);
	tagNewSensor.set_type(byType);
	tagNewSensor.set_slope_id(wSlopeID);
	tagNewSensor.set_state(0);
	tagNewSensor.set_longitude(dLongitude);
	tagNewSensor.set_latitude(dLatitude);
	tagNewSensor.set_url(strUrl);
	tagNewSensor.set_description(strDescription);

	SendAppMsg(APP_SERVER_NET_Protocol::S2APP::s2app_add_sensor, tagNewSensor);
}

void CAppClient::DBResopndDelSensorResult(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData)
{
	UINT	uCol			= 0;
	BYTE	byResult		= 0;
	BYTE	byResultCount	= pResultSet->GetResultCount();
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

	pResult1->GetData(0, 0, byResult);

	APP_SERVER_NET_Protocol::S2APP_Del_Sensor	tagDelSensor;

	tagDelSensor.set_result(byResult);

	SendAppMsg(APP_SERVER_NET_Protocol::S2APP::s2app_del_sensor, tagDelSensor);
}

void CAppClient::DBResopndUpdateSensorResult(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData)
{
	UINT	uCol					= 0;
	BYTE	byResult				= 0;
	UINT	uSensorID				= 0;
	WORD	wSceneID				= 0;
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
		APP_SERVER_NET_Protocol::S2APP_ERROR	tagError;
		tagError.set_error_code(byResult);

		SendAppMsg(APP_SERVER_NET_Protocol::S2APP::s2app_error, tagError);

		return;
	}

	APP_SERVER_NET_Protocol::S2APP_Update_Sensor	tagUpdateSensor;

	uCol	= 0;

	pResult2->GetData(0, uCol++, uSensorID);
	pResult2->GetData(0, uCol++, wSceneID);
	pResult2->GetData(0, uCol++, byType);
	pResult2->GetData(0, uCol++, wSlopeID);
	pResult2->GetData(0, uCol++, dLongitude);
	pResult2->GetData(0, uCol++, dLatitude);
	pResult2->GetData(0, uCol++, strUrl, sizeof(strUrl));
	pResult2->GetData(0, uCol++, strDescription, sizeof(strDescription));

	tagUpdateSensor.set_id(uSensorID);
	tagUpdateSensor.set_scene_id(wSceneID);
	tagUpdateSensor.set_type(byType);
	tagUpdateSensor.set_slope_id(wSlopeID);
	tagUpdateSensor.set_state(0);
	tagUpdateSensor.set_longitude(dLongitude);
	tagUpdateSensor.set_latitude(dLatitude);
	tagUpdateSensor.set_url(strUrl);
	tagUpdateSensor.set_description(strDescription);

	SendAppMsg(APP_SERVER_NET_Protocol::S2APP::s2app_update_sensor, tagUpdateSensor);
}

void CAppClient::DBResopndModifyPasswordResult(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData)
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

	BYTE	byResult	= 0;

	pResult1->GetData(0, 0, byResult);

	APP_SERVER_NET_Protocol::S2APP_Modify_Password_Result	tagModifyPassword;
	tagModifyPassword.set_result(byResult);


	SendAppMsg(APP_SERVER_NET_Protocol::S2APP::s2app_modify_password_result, tagModifyPassword);
}

void CAppClient::DBResopndFindSlopeResult(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData)
{
	UINT	uCol				= 0;
	WORD	wSlopeID			= 0;
	WORD	wSceneID			= 0;
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
		return;

	APP_SERVER_NET_Protocol::S2App_Slope_List	tagSlopeList;

	for (auto uRow = 0; uRow < pResult1->GetRowCount(); ++uRow)
	{
		APP_SERVER_NET_Protocol::S2App_Slope_List::SlopeData	*pSlopeData = tagSlopeList.add_slope_list();
		if (nullptr == pSlopeData)
			continue;

		uCol	= 0;

		pResult1->GetData(uRow, uCol++, wSlopeID);
		pResult1->GetData(uRow, uCol++, wSceneID);
		pResult1->GetData(uRow, uCol++, bySlopeType);
		pResult1->GetData(uRow, uCol++, strSlopeName, sizeof(strSlopeName));
		pResult1->GetData(uRow, uCol++, dLongitude);
		pResult1->GetData(uRow, uCol++, dLatitude);
		pResult1->GetData(uRow, uCol++, byState);
		pResult1->GetData(uRow, uCol++, strUrl, sizeof(strUrl));

		pSlopeData->set_id(wSlopeID);
		pSlopeData->set_scene_id(wSceneID);
		pSlopeData->set_type(bySlopeType);
		pSlopeData->set_name(strSlopeName);
		pSlopeData->set_longitude(dLongitude);
		pSlopeData->set_latitude(dLatitude);
		pSlopeData->set_state(byState);
		pSlopeData->set_url(strUrl);
	}

	SendAppMsg(APP_SERVER_NET_Protocol::S2APP::s2app_slope_list, tagSlopeList);
}

void CAppClient::DBResopndFindSensorResult(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData)
{
	UINT	uCol			= 0;
	UINT	uSensorID		= 0;
	WORD	wSceneID		= 0;
	BYTE	bySensorType	= 0;
	double	dCurValue1		= 0.0;
	double	dCurValue2		= 0.0;
	double	dCurValue3		= 0.0;
	double	dCurValue4		= 0.0;
	double	dAvgValue1		= 0.0;
	double	dAvgValue2		= 0.0;
	double	dAvgValue3		= 0.0;
	double	dOffsetValue1	= 0.0;
	double	dOffsetValue2	= 0.0;
	double	dOffsetValue3	= 0.0;
	BYTE	byState			= 0;
	WORD	wSlopeID		= 0;
	double	dLongitude		= 0.0;
	double	dLatitude		= 0.0;
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
		return;

	APP_SERVER_NET_Protocol::S2App_Sensor_List	tagSensorList;

	for (auto uRow = 0; uRow < pResult1->GetRowCount(); ++uRow)
	{
		APP_SERVER_NET_Protocol::S2App_Sensor_List::SensorData	*pSensor = tagSensorList.add_sensor_list();
		if (nullptr == pSensor)
			continue;

		uCol	= 0;

		pResult1->GetData(uRow, uCol++, uSensorID);
		pResult1->GetData(uRow, uCol++, wSceneID);
		pResult1->GetData(uRow, uCol++, bySensorType);
		pResult1->GetData(uRow, uCol++, dCurValue1);
		pResult1->GetData(uRow, uCol++, dCurValue2);
		pResult1->GetData(uRow, uCol++, dCurValue3);
		pResult1->GetData(uRow, uCol++, dCurValue4);
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
		pSensor->set_scene_id(wSceneID);
		pSensor->set_type(bySensorType);
		pSensor->set_cur_value1(dCurValue1);
		pSensor->set_cur_value2(dCurValue2);
		pSensor->set_cur_value3(dCurValue3);
		pSensor->set_cur_value4(dCurValue4);
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

void CAppClient::DBResopndLoadAuthor(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData)
{
	WORD	wAuthorityID		= 0;
	WORD	wParentID			= 0;
	char	strUrl[256]			= {0};
	char	strIconUrl[256]		= {0};
	char	strDescption[256]	= {0};
	UINT	uCol				= 0;

	IMysqlResult	*pResult1 = pResultSet->GetMysqlResult(0);

	if (nullptr == pResult1)
		return;

	APP_SERVER_NET_Protocol::S2APP_Authority_List	tagAuthorityList;

	for (auto uRow = 0; uRow < pResult1->GetRowCount(); ++uRow)
	{
		APP_SERVER_NET_Protocol::S2APP_Authority_List::AuthorityData	*pAuthorData = tagAuthorityList.add_authority_list();
		if (nullptr == pAuthorData)
			continue;

		uCol = 0;

		pResult1->GetData(uRow, uCol++, wAuthorityID);
		pResult1->GetData(uRow, uCol++, wParentID);
		pResult1->GetData(uRow, uCol++, strUrl, sizeof(strUrl));
		pResult1->GetData(uRow, uCol++, strIconUrl, sizeof(strIconUrl));
		pResult1->GetData(uRow, uCol++, strDescption, sizeof(strDescption));

		pAuthorData->set_authority_id(wAuthorityID);
		pAuthorData->set_parent_id(wParentID);
		pAuthorData->set_url(strUrl);
		pAuthorData->set_icon_url(strIconUrl);
		pAuthorData->set_description(strDescption);
	}

	SendAppMsg(APP_SERVER_NET_Protocol::S2APP::s2app_authority_list, tagAuthorityList);
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
