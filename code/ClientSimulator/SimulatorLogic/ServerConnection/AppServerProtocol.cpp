#include "stdafx.h"
#include "AppServerConnection.h"

CAppServerConnection::pfnProtocolFunc CAppServerConnection::m_ProtocolFunc[APP_SERVER_NET_Protocol::S2APP::s2app_max] =
{
	&CAppServerConnection::RecvLoginResult,
	&CAppServerConnection::RecvSlopeList,
	&CAppServerConnection::RecvSensorList,
	&CAppServerConnection::RecvSensorHistory,
	&CAppServerConnection::RecvAllList,
};

void CAppServerConnection::ProcessNetPack(const void *pPack, const unsigned int uPackLen)
{
	if (nullptr == pPack)
		return;

	BYTE			byProtocol	= 0;

	byProtocol	= *((BYTE*)pPack);

	if (byProtocol >= APP_SERVER_NET_Protocol::S2APP::s2app_max)
	{
		g_pFileLog->WriteLog("[%s][%d] Server Connection Invalid Protocol[%hhu]\n", __FILE__, __LINE__, byProtocol);
		return;
	}

	(this->*m_ProtocolFunc[byProtocol])(pPack, uPackLen);
}

void CAppServerConnection::RecvLoginResult(const void *pPack, const unsigned int uPackLen)
{
	APP_SERVER_NET_Protocol::S2App_Login_Result	tagLoginResult;

	ParseFromPackage(tagLoginResult, pPack, uPackLen);
}

void CAppServerConnection::RecvSlopeList(const void *pPack, const unsigned int uPackLen)
{
	APP_SERVER_NET_Protocol::S2App_Slope_List	tagSlopeList;

	ParseFromPackage(tagSlopeList, pPack, uPackLen);
}

void CAppServerConnection::RecvSensorList(const void *pPack, const unsigned int uPackLen)
{
	APP_SERVER_NET_Protocol::S2App_Sensor_List	tagSensorList;

	ParseFromPackage(tagSensorList, pPack, uPackLen);
}

void CAppServerConnection::RecvSensorHistory(const void *pPack, const unsigned int uPackLen)
{
	APP_SERVER_NET_Protocol::S2App_Sensor_History	tagSensorHistory;

	ParseFromPackage(tagSensorHistory, pPack, uPackLen);
}

void CAppServerConnection::RecvAllList(const void *pPack, const unsigned int uPackLen)
{
}

void CAppServerConnection::ParseFromPackage(google::protobuf::Message &tagMsg, const void *pPack, const unsigned int uPackLen)
{
	tagMsg.ParseFromArray((BYTE*)pPack+sizeof(BYTE), uPackLen-sizeof(BYTE));
}
