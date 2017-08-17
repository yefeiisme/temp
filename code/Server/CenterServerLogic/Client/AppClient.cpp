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
	&CAppClient::DefaultProtocolFunc,
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

void CAppClient::ProcessDBPack(SMysqlRespond *pRespond, SMysqlDataHead *pDataHead)
{
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
			g_pFileLog->WriteLog("[%s][%d] App Client[%u] Invalid Protocol[%hhu]\n", __FUNCTION__, __LINE__, m_uUniqueID, byProtocol);
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

	char			strBuffer[0xffff] = {0};
	SMysqlRequest	*pRequet	= (SMysqlRequest*)strBuffer;
	pRequet->byOpt				= SENSOR_DB_VERIFY_ACCOUNT;
	pRequet->uClientID			= m_uUniqueID;
	pRequet->uClientIndex		= m_uIndex;
	pRequet->byClientType		= APP_CLIENT;

	g_ICenterServer.SendDBRequest(strBuffer, 0);
}

void CAppClient::RecvRequestSlopeList(const void *pPack, const unsigned int uPackLen)
{
	APP_SERVER_NET_Protocol::APP2S_Request_Slope_List	tagRequest;
	BYTE	*pLoginInfo = (BYTE*)pPack + sizeof(BYTE);
	tagRequest.ParseFromArray(pLoginInfo, uPackLen - sizeof(BYTE));

	char			strBuffer[0xffff] = {0};
	SMysqlRequest	*pRequet	= (SMysqlRequest*)strBuffer;
	pRequet->byOpt			= SENSOR_DB_SLOPE_LIST;
	pRequet->uClientID		= m_uUniqueID;
	pRequet->uClientIndex	= m_uIndex;
	pRequet->byClientType	= APP_CLIENT;

	// 组织请求的结构，向数据库线程发送请示
	// ...
}

void CAppClient::RecvRequestSensorList(const void *pPack, const unsigned int uPackLen)
{
	APP_SERVER_NET_Protocol::APP2S_Request_Sensor_List	tagRequest;
	BYTE	*pLoginInfo = (BYTE*)pPack + sizeof(BYTE);
	tagRequest.ParseFromArray(pLoginInfo, uPackLen - sizeof(BYTE));

	char			strBuffer[0xffff] = {0};
	SMysqlRequest	*pRequet	= (SMysqlRequest*)strBuffer;
	pRequet->byOpt			= SENSOR_DB_SENSOR_LIST;
	pRequet->uClientID		= m_uUniqueID;
	pRequet->uClientIndex	= m_uIndex;
	pRequet->byClientType	= APP_CLIENT;

	// 组织请求的结构，向数据库线程发送请示
	// ...
}

void CAppClient::RecvRequestSensorHistory(const void *pPack, const unsigned int uPackLen)
{
	APP_SERVER_NET_Protocol::APP2S_Request_Sensor_History	tagRequest;
	BYTE	*pLoginInfo = (BYTE*)pPack + sizeof(BYTE);
	tagRequest.ParseFromArray(pLoginInfo, uPackLen - sizeof(BYTE));

	char			strBuffer[0xffff] = {0};
	SMysqlRequest	*pRequet	= (SMysqlRequest*)strBuffer;
	pRequet->byOpt			= SENSOR_DB_SENSOR_HISTORY;
	pRequet->uClientID		= m_uUniqueID;
	pRequet->uClientIndex	= m_uIndex;
	pRequet->byClientType	= APP_CLIENT;

	// 组织请求的结构，向数据库线程发送请示
	// ...
}
