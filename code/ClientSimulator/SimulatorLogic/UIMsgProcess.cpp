#include "stdafx.h"
#include "SimulatorLogic.h"
#include "ServerConnection/AppServerConnection.h"
#include "ServerConnection/WebServerConnection.h"

void CSimulatorLogic::ProcessRequest()
{
	const void		*pPack		= nullptr;
	unsigned int	uPackLen	= 0;

	while (nullptr != (pPack = m_pRBRequest->RcvPack(uPackLen)))
	{
		BYTE byProtocol	= *((BYTE*)pPack);

		(this->*m_ProtocolFunc[byProtocol])(pPack, uPackLen);
	};
}

void CSimulatorLogic::RecvAppLogin(const void *pPack, const unsigned int uPackLen)
{
	U2L_APP_LOGIN	*pUIRequest	= (U2L_APP_LOGIN*)pPack;

	char	strBuffer[0xff];
	BYTE	*pProtocol	= (BYTE*)strBuffer;
	*pProtocol	= APP_SERVER_NET_Protocol::APP2S::app2s_login;

	APP_SERVER_NET_Protocol::App2S_Login	tagLogin;
	tagLogin.set_account(pUIRequest->strAccount);
	tagLogin.set_password(pUIRequest->strPassword);

	if (tagLogin.ByteSize() > sizeof(strBuffer) - sizeof(BYTE))
		return;

	tagLogin.SerializePartialToArray(strBuffer + sizeof(BYTE), tagLogin.ByteSize());

	m_pAppServerConnList[0].PutPack(strBuffer, sizeof(BYTE) + tagLogin.ByteSize());
}

void CSimulatorLogic::RecvWebLogin(const void *pPack, const unsigned int uPackLen)
{
	U2L_WEB_LOGIN	*pUIRequest	= (U2L_WEB_LOGIN*)pPack;

	char	strBuffer[0xff];
	BYTE	*pProtocol	= (BYTE*)strBuffer;
	*pProtocol	= WEB_SERVER_NET_Protocol::WEB2S::web2s_login;

	WEB_SERVER_NET_Protocol::Web2S_Login	tagLogin;
	tagLogin.set_account(pUIRequest->strAccount);
	tagLogin.set_password(pUIRequest->strPassword);

	if (tagLogin.ByteSize() > sizeof(strBuffer)-sizeof(BYTE))
		return;

	tagLogin.SerializePartialToArray(strBuffer + sizeof(BYTE), tagLogin.ByteSize());

	m_pWebServerConnList[0].PutPack(strBuffer, sizeof(BYTE)+tagLogin.ByteSize());
}

void CSimulatorLogic::RecvAppSlopeList(const void *pPack, const unsigned int uPackLen)
{
	U2L_APP_SLOPE_LIST	*pUIRequest = (U2L_APP_SLOPE_LIST*)pPack;

	char	strBuffer[0xff];
	BYTE	*pProtocol	= (BYTE*)strBuffer;
	*pProtocol = APP_SERVER_NET_Protocol::APP2S::app2s_request_slope_list;

	APP_SERVER_NET_Protocol::APP2S_Request_Slope_List	tagRequest;
	tagRequest.set_server_id(pUIRequest->wServerID);

	if (tagRequest.ByteSize() > sizeof(strBuffer)-sizeof(BYTE))
		return;

	tagRequest.SerializePartialToArray(strBuffer + sizeof(BYTE), tagRequest.ByteSize());

	m_pAppServerConnList[0].PutPack(strBuffer, sizeof(BYTE)+tagRequest.ByteSize());
}

void CSimulatorLogic::RecvWebSlopeList(const void *pPack, const unsigned int uPackLen)
{
	U2L_WEB_SLOPE_LIST	*pUIRequest = (U2L_WEB_SLOPE_LIST*)pPack;

	char	strBuffer[0xff];
	BYTE	*pProtocol = (BYTE*)strBuffer;
	*pProtocol = WEB_SERVER_NET_Protocol::WEB2S::web2s_request_slope_list;

	WEB_SERVER_NET_Protocol::WEB2S_Request_Slope_List	tagRequest;
	tagRequest.set_server_id(pUIRequest->wServerID);

	if (tagRequest.ByteSize() > sizeof(strBuffer)-sizeof(BYTE))
		return;

	tagRequest.SerializePartialToArray(strBuffer + sizeof(BYTE), tagRequest.ByteSize());

	m_pWebServerConnList[0].PutPack(strBuffer, sizeof(BYTE)+tagRequest.ByteSize());
}

void CSimulatorLogic::RecvAppSensorList(const void *pPack, const unsigned int uPackLen)
{
	U2L_APP_SENSOR_LIST	*pUIRequest = (U2L_APP_SENSOR_LIST*)pPack;

	char	strBuffer[0xff];
	BYTE	*pProtocol = (BYTE*)strBuffer;
	*pProtocol = APP_SERVER_NET_Protocol::APP2S::app2s_request_sensor_list;

	APP_SERVER_NET_Protocol::APP2S_Request_Sensor_List	tagRequest;
	tagRequest.set_slope_id(pUIRequest->wSlopeID);
	tagRequest.set_sensor_type(0);

	if (tagRequest.ByteSize() > sizeof(strBuffer)-sizeof(BYTE))
		return;

	tagRequest.SerializePartialToArray(strBuffer + sizeof(BYTE), tagRequest.ByteSize());

	m_pAppServerConnList[0].PutPack(strBuffer, sizeof(BYTE)+tagRequest.ByteSize());
}

void CSimulatorLogic::RecvWebSensorList(const void *pPack, const unsigned int uPackLen)
{
	U2L_WEB_SENSOR_LIST	*pUIRequest = (U2L_WEB_SENSOR_LIST*)pPack;

	char	strBuffer[0xff];
	BYTE	*pProtocol = (BYTE*)strBuffer;
	*pProtocol = WEB_SERVER_NET_Protocol::WEB2S::web2s_request_sensor_list;

	WEB_SERVER_NET_Protocol::WEB2S_Request_Sensor_List	tagRequest;
	tagRequest.set_slope_id(pUIRequest->wSlopeID);

	if (tagRequest.ByteSize() > sizeof(strBuffer)-sizeof(BYTE))
		return;

	tagRequest.SerializePartialToArray(strBuffer + sizeof(BYTE), tagRequest.ByteSize());

	m_pWebServerConnList[0].PutPack(strBuffer, sizeof(BYTE)+tagRequest.ByteSize());
}

void CSimulatorLogic::RecvAppSensorHistory(const void *pPack, const unsigned int uPackLen)
{
	U2L_APP_SENSOR_HISTORY	*pUIRequest = (U2L_APP_SENSOR_HISTORY*)pPack;

	char	strBuffer[0xff];
	BYTE	*pProtocol = (BYTE*)strBuffer;
	*pProtocol = APP_SERVER_NET_Protocol::APP2S::app2s_request_sensor_history;

	APP_SERVER_NET_Protocol::APP2S_Request_Sensor_History	tagRequest;
	tagRequest.set_sensor_id(pUIRequest->uSensorID);
	tagRequest.set_begin_time(pUIRequest->nBeginTime);
	tagRequest.set_end_time(pUIRequest->nEndTime);

	if (tagRequest.ByteSize() > sizeof(strBuffer)-sizeof(BYTE))
		return;

	tagRequest.SerializePartialToArray(strBuffer + sizeof(BYTE), tagRequest.ByteSize());

	m_pAppServerConnList[0].PutPack(strBuffer, sizeof(BYTE)+tagRequest.ByteSize());
}

void CSimulatorLogic::RecvWebSensorHistory(const void *pPack, const unsigned int uPackLen)
{
	U2L_WEB_SENSOR_HISTORY	*pUIRequest = (U2L_WEB_SENSOR_HISTORY*)pPack;

	char	strBuffer[0xff];
	BYTE	*pProtocol = (BYTE*)strBuffer;
	*pProtocol = WEB_SERVER_NET_Protocol::WEB2S::web2s_request_sensor_history;

	WEB_SERVER_NET_Protocol::WEB2S_Request_Sensor_History	tagRequest;
	tagRequest.set_sensor_id(pUIRequest->uSensorID);
	tagRequest.set_begin_time(pUIRequest->nBeginTime);
	tagRequest.set_end_time(pUIRequest->nEndTime);

	if (tagRequest.ByteSize() > sizeof(strBuffer)-sizeof(BYTE))
		return;

	tagRequest.SerializePartialToArray(strBuffer + sizeof(BYTE), tagRequest.ByteSize());

	m_pWebServerConnList[0].PutPack(strBuffer, sizeof(BYTE)+tagRequest.ByteSize());
}
