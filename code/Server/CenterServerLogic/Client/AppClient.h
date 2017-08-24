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
	void					ProcessDBPack(SMysqlRespond &pRespond, SMysqlDataHead *pDataHead, IQueryResult *pResult);
private:
	void					ProcessNetPack();

	void					DefaultProtocolFunc(const void *pPack, const unsigned int uPackLen);

	void					RecvLogin(const void *pPack, const unsigned int uPackLen);
	void					RecvRequestSlopeList(const void *pPack, const unsigned int uPackLen);
	void					RecvRequestSensorList(const void *pPack, const unsigned int uPackLen);
	void					RecvRequestSensorHistory(const void *pPack, const unsigned int uPackLen);
	void					RecvRequestAllList(const void *pPack, const unsigned int uPackLen);
private:
	void					DBResopndLoginResult(SMysqlRespond &pRespond, SMysqlDataHead *pDataHead, IQueryResult *pResult);
	void					DBResopndSlopeList(SMysqlRespond &pRespond, SMysqlDataHead *pDataHead, IQueryResult *pResult);
	void					DBResopndSensorList(SMysqlRespond &pRespond, SMysqlDataHead *pDataHead, IQueryResult *pResult);
	void					DBResopndSensorHistory(SMysqlRespond &pRespond, SMysqlDataHead *pDataHead, IQueryResult *pResult);
	void					DBResopndAllList(SMysqlRespond &pRespond, SMysqlDataHead *pDataHead, IQueryResult *pResult);
private:
	typedef void			(CAppClient::*pfnProtocolFunc)(const void *pPack, const unsigned int uPackLen);
	static pfnProtocolFunc	m_ProtocolFunc[APP_SERVER_NET_Protocol::APP2S::app2s_max];

	typedef void			(CAppClient::*pfnDBRespondFunc)(SMysqlRespond &pRespond, SMysqlDataHead *pDataHead, IQueryResult *pResult);
	static pfnDBRespondFunc	m_pfnDBRespondFunc[SENSOR_DB_OPT_MAX];
private:
};

#endif
