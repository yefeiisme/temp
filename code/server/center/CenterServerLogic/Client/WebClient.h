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
	void					RecvModifyPassword(const void *pPack, const unsigned int uPackLen);
	void					RecvFindSlope(const void *pPack, const unsigned int uPackLen);
	void					RecvFindSensor(const void *pPack, const unsigned int uPackLen);

	void					RecvLoadUserList(const void *pPack, const unsigned int uPackLen);
	void					RecvCreateUser(const void *pPack, const unsigned int uPackLen);
	void					RecvModifyUser(const void *pPack, const unsigned int uPackLen);
	void					RecvRemoveUser(const void *pPack, const unsigned int uPackLen);
	void					RecvLoadGroupList(const void *pPack, const unsigned int uPackLen);

	void					RecvCreateGroup(const void *pPack, const unsigned int uPackLen);
	void					RecvAddUserToGroup(const void *pPack, const unsigned int uPackLen);
	void					RecvRemoveUserFromGroup(const void *pPack, const unsigned int uPackLen);
	void					RecvModifyGroup(const void *pPack, const unsigned int uPackLen);
	void					RecvRemoveGroup(const void *pPack, const unsigned int uPackLen);

	void					RecvLoadAuthority(const void *pPack, const unsigned int uPackLen);
	void					RecvModifyAlarmValue(const void *pPack, const unsigned int uPackLen);
	void					RecvLoadAlarmList(const void *pPack, const unsigned int uPackLen);
	void					RecvStartSlope(const void *pPack, const unsigned int uPackLen);
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
	void					DBResopndModifyPasswordResult(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData);
	void					DBResopndFindSlopeResult(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData);
	void					DBResopndFindSensorResult(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData);
	void					DBResopndLoadUserList(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData);

	void					DBResopndCreateUser(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData);
	void					DBResopndModifyUser(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData);
	void					DBResopndRemoveUser(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData);
	void					DBResopndLoadGroupList(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData);
	void					DBResopndCreateGroup(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData);

	void					DBResopndAddUserToGroup(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData);
	void					DBResopndRemoveUserFromGroup(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData);
	void					DBResopndModifyGroup(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData);
	void					DBResopndRemoveGroup(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData);
	void					DBResopndLoadAuthor(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData);

	void					DBResopndAddSensorData(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData);
	void					DBResopndModifyAlarmValue(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData);
	void					DBResopndLoadAlarmValue(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData);
	void					DBResopndStartSlopeResult(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData);
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
