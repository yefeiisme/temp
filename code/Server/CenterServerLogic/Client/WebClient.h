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
	void					ProcessDBPack(SMysqlRespond &pRespond, IQueryResult *pResult);
private:
	void					ProcessNetPack();

	void					DefaultProtocolFunc(const void *pPack, const unsigned int uPackLen);

	void					RecvLogin(const void *pPack, const unsigned int uPackLen);
	void					RecvRequestSlopeList(const void *pPack, const unsigned int uPackLen);
	void					RecvRequestSensorList(const void *pPack, const unsigned int uPackLen);
	void					RecvRequestSensorHistory(const void *pPack, const unsigned int uPackLen);
	void					RecvRequestAllList(const void *pPack, const unsigned int uPackLen);
private:
	void					DBResopndLoginResult(SMysqlRespond &pRespond, IQueryResult *pResult);
	void					DBResopndSlopeList(SMysqlRespond &pRespond, IQueryResult *pResult);
	void					DBResopndSensorList(SMysqlRespond &pRespond, IQueryResult *pResult);
	void					DBResopndSensorHistory(SMysqlRespond &pRespond, IQueryResult *pResult);
	void					DBResopndAllList(SMysqlRespond &pRespond, IQueryResult *pResult);
private:
	void					SendWebLoginResult(WEB_SERVER_NET_Protocol::S2Web_Login_Result &tagLoginResult);
	void					SendWebSlopeList(WEB_SERVER_NET_Protocol::S2Web_Slope_List &tagSlopeList);
	void					SendWebSensorList(WEB_SERVER_NET_Protocol::S2Web_Sensor_List &tagSensorList);
	void					SendWebSensorHistory(WEB_SERVER_NET_Protocol::S2Web_Sensor_History &tagSensorHistory);
private:
	typedef void			(CWebClient::*pfnProtocolFunc)(const void *pPack, const unsigned int uPackLen);
	static pfnProtocolFunc	m_ProtocolFunc[WEB_SERVER_NET_Protocol::WEB2S::web2s_max];

	typedef void			(CWebClient::*pfnDBRespondFunc)(SMysqlRespond &pRespond, IQueryResult *pResult);
	static pfnDBRespondFunc	m_pfnDBRespondFunc[SENSOR_DB_OPT_MAX];
private:
};

#endif
