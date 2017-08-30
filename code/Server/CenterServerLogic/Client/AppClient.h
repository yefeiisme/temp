#ifndef __APP_CLIENT_H_
#define __APP_CLIENT_H_

#include "IMysqlQuery.h"
#include "Client.h"
#include "App_Server_Protocol.pb.h"
#include "../SensorDB/SensorDBOperation.h"

class CAppClient : public CClient
{
public:
	CAppClient();
	~CAppClient();

	void					DoAction();
	void					ProcessDBPack(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData);
private:
	void					ProcessNetPack();

	void					DefaultProtocolFunc(const void *pPack, const unsigned int uPackLen);

	void					RecvLogin(const void *pPack, const unsigned int uPackLen);
	void					RecvRequestSlopeList(const void *pPack, const unsigned int uPackLen);
	void					RecvRequestSensorList(const void *pPack, const unsigned int uPackLen);
	void					RecvRequestSensorHistory(const void *pPack, const unsigned int uPackLen);
	void					RecvRequestAllList(const void *pPack, const unsigned int uPackLen);
private:
	void					DBResopndLoginResult(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData);
	void					DBResopndSlopeList(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData);
	void					DBResopndSensorList(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData);
	void					DBResopndSensorHistory(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData);
	void					DBResopndAllList(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData);
private:
	void					SendAppLoginResult(APP_SERVER_NET_Protocol::S2App_Login_Result &tagLoginResult);
	void					SendAppSlopeList(APP_SERVER_NET_Protocol::S2App_Slope_List &tagSlopeList);
	void					SendAppSensorList(APP_SERVER_NET_Protocol::S2App_Sensor_List &tagSensorList);
	void					SendAppSensorHistory(APP_SERVER_NET_Protocol::S2App_Sensor_History &tagSensorHistory);
private:
	typedef void			(CAppClient::*pfnProtocolFunc)(const void *pPack, const unsigned int uPackLen);
	static pfnProtocolFunc	m_ProtocolFunc[APP_SERVER_NET_Protocol::APP2S::app2s_max];

	typedef void			(CAppClient::*pfnDBRespondFunc)(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData);
	static pfnDBRespondFunc	m_pfnDBRespondFunc[SENSOR_DB_OPT_MAX];
private:
};

#endif
