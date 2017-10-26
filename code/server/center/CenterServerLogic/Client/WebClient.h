#ifndef __WEB_CLIENT_H_
#define __WEB_CLIENT_H_

#include "IMysqlQuery.h"
#include "Client.h"
#include "Web_Server_Protocol.pb.h"
#include "../SensorDB/SensorDBOperation.h"

class CWebClient : public CClient
{
public:
	CWebClient();
	~CWebClient();

	void					DoAction();
	void					ProcessDBPack(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData);
private:
	void					ProcessNetPack();

	void					DefaultProtocolFunc(const void *pPack, const unsigned int uPackLen);

	void					RecvLogin(const void *pPack, const unsigned int uPackLen);
	void					RecvRequestSlopeList(const void *pPack, const unsigned int uPackLen);
	void					RecvRequestSensorList(const void *pPack, const unsigned int uPackLen);
	void					RecvRequestSensorHistory(const void *pPack, const unsigned int uPackLen);
	void					RecvPing(const void *pPack, const unsigned int uPackLen);

	void					RecvRequestAllList(const void *pPack, const unsigned int uPackLen);
	void					RecvAddSlope(const void *pPack, const unsigned int uPackLen);
	void					RecvDelSlope(const void *pPack, const unsigned int uPackLen);
	void					RecvUpdateSlope(const void *pPack, const unsigned int uPackLen);
	void					RecvAddSensor(const void *pPack, const unsigned int uPackLen);

	void					RecvDelSensor(const void *pPack, const unsigned int uPackLen);
	void					RecvUpdateSensor(const void *pPack, const unsigned int uPackLen);
private:
	void					DBResopndLoginResult(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData);
	void					DBResopndSlopeList(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData);
	void					DBResopndSensorList(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData);
	void					DBResopndSensorHistory(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData);
	void					DBResopndAllList(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData);

	void					DBResopndAddSlopeResult(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData);
	void					DBResopndDelSlopeResult(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData);
	void					DBResopndUpdateSlopeResult(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData);
	void					DBResopndAddSensorResult(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData);
	void					DBResopndDelSensorResult(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData);

	void					DBResopndUpdateSensorResult(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData);
private:
	void					SendWebMsg(const BYTE byProtocol, google::protobuf::Message &tagMsg);
private:
	typedef void			(CWebClient::*pfnProtocolFunc)(const void *pPack, const unsigned int uPackLen);
	static pfnProtocolFunc	m_ProtocolFunc[WEB_SERVER_NET_Protocol::WEB2S::web2s_max];

	typedef void			(CWebClient::*pfnDBRespondFunc)(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData);
	static pfnDBRespondFunc	m_pfnDBRespondFunc[SENSOR_DB_OPT_MAX];
private:
};

#endif
