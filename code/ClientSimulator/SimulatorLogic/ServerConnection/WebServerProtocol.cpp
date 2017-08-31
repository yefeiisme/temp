#include "stdafx.h"
#include "WebServerConnection.h"

CWebServerConnection::pfnProtocolFunc CWebServerConnection::m_ProtocolFunc[WEB_SERVER_NET_Protocol::S2WEB::s2web_max] =
{
	&CWebServerConnection::RecvLoginResult,
	&CWebServerConnection::RecvSlopeList,
	&CWebServerConnection::RecvSensorList,
	&CWebServerConnection::RecvSensorHistory,
	&CWebServerConnection::RecvAllList,
};

void CWebServerConnection::ProcessNetPack(const void *pPack, const unsigned int uPackLen)
{
	if (nullptr == pPack)
		return;

	BYTE			byProtocol	= 0;

	byProtocol	= *((BYTE*)pPack);

	if (byProtocol >= WEB_SERVER_NET_Protocol::S2WEB::s2web_max)
	{
		g_pFileLog->WriteLog("[%s][%d] Server Connection Invalid Protocol[%hhu]\n", __FILE__, __LINE__, byProtocol);
		return;
	}

	(this->*m_ProtocolFunc[byProtocol])(pPack, uPackLen);
}

void CWebServerConnection::RecvLoginResult(const void *pPack, const unsigned int uPackLen)
{
	WEB_SERVER_NET_Protocol::S2Web_Login_Result	tagLoginResult;

	ParseFromPackage(tagLoginResult, pPack, uPackLen);
}

void CWebServerConnection::RecvSlopeList(const void *pPack, const unsigned int uPackLen)
{
	WEB_SERVER_NET_Protocol::S2Web_Slope_List	tagSlopeList;

	ParseFromPackage(tagSlopeList, pPack, uPackLen);
}

void CWebServerConnection::RecvSensorList(const void *pPack, const unsigned int uPackLen)
{
	WEB_SERVER_NET_Protocol::S2Web_Sensor_List	tagSensorList;

	ParseFromPackage(tagSensorList, pPack, uPackLen);
}

void CWebServerConnection::RecvSensorHistory(const void *pPack, const unsigned int uPackLen)
{
	WEB_SERVER_NET_Protocol::S2Web_Sensor_History	tagSensorHistory;

	ParseFromPackage(tagSensorHistory, pPack, uPackLen);
}

void CWebServerConnection::RecvAllList(const void *pPack, const unsigned int uPackLen)
{
}

void CWebServerConnection::ParseFromPackage(google::protobuf::Message &tagMsg, const void *pPack, const unsigned int uPackLen)
{
	tagMsg.ParseFromArray((BYTE*)pPack + sizeof(BYTE), uPackLen - sizeof(BYTE));
}
