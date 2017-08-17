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
			g_pFileLog->WriteLog("[%s][%d] App Client[%u] Invalid Protocol[%hhu]\n", __FUNCTION__, __LINE__, m_uUniqueID, byProtocol);
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

	char			strBuffer[0xffff] = {0};
	SMysqlRequest	*pRequet	= (SMysqlRequest*)strBuffer;
	pRequet->byOpt			= SENSOR_DB_VERIFY_ACCOUNT;
	pRequet->uClientID		= m_uUniqueID;
	pRequet->uClientIndex	= m_uIndex;
	pRequet->byClientType	= WEB_CLIENT;
	// 组织请求的结构，向数据库线程发送请示
	// ...
}

void CWebClient::RecvRequestSlopeList(const void *pPack, const unsigned int uPackLen)
{
	WEB_SERVER_NET_Protocol::WEB2S_Request_Slope_List	tagRequest;
	BYTE	*pLoginInfo = (BYTE*)pPack + sizeof(BYTE);
	tagRequest.ParseFromArray(pLoginInfo, uPackLen - sizeof(BYTE));

	char			strBuffer[0xffff] = {0};
	SMysqlRequest	*pRequet	= (SMysqlRequest*)strBuffer;
	pRequet->byOpt			= SENSOR_DB_SLOPE_LIST;
	pRequet->uClientID		= m_uUniqueID;
	pRequet->uClientIndex	= m_uIndex;
	pRequet->byClientType	= WEB_CLIENT;

	// 组织请求的结构，向数据库线程发送请示
	// ...
}

void CWebClient::RecvRequestSensorList(const void *pPack, const unsigned int uPackLen)
{
	WEB_SERVER_NET_Protocol::WEB2S_Request_Sensor_List	tagRequest;
	BYTE	*pLoginInfo = (BYTE*)pPack + sizeof(BYTE);
	tagRequest.ParseFromArray(pLoginInfo, uPackLen - sizeof(BYTE));

	char			strBuffer[0xffff] ={ 0 };
	SMysqlRequest	*pRequet	= (SMysqlRequest*)strBuffer;
	pRequet->byOpt			= SENSOR_DB_SENSOR_LIST;
	pRequet->uClientID		= m_uUniqueID;
	pRequet->uClientIndex	= m_uIndex;
	pRequet->byClientType	= WEB_CLIENT;

	// 组织请求的结构，向数据库线程发送请示
	// ...
}

void CWebClient::RecvRequestSensorHistory(const void *pPack, const unsigned int uPackLen)
{
	WEB_SERVER_NET_Protocol::WEB2S_Request_Sensor_History	tagRequest;
	BYTE	*pLoginInfo = (BYTE*)pPack + sizeof(BYTE);
	tagRequest.ParseFromArray(pLoginInfo, uPackLen - sizeof(BYTE));

	char			strBuffer[0xffff] = {0};
	SMysqlRequest	*pRequet	= (SMysqlRequest*)strBuffer;
	pRequet->byOpt			= SENSOR_DB_SENSOR_HISTORY;
	pRequet->uClientID		= m_uUniqueID;
	pRequet->uClientIndex	= m_uIndex;
	pRequet->byClientType	= WEB_CLIENT;

	// 组织请求的结构，向数据库线程发送请示
	// ...
}
