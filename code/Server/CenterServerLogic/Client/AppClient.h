#ifndef __APP_CLIENT_H_
#define __APP_CLIENT_H_

#include "IMysqlQuery.h"
#include "Client.h"
#include "App_Server_Protocol.pb.h"

class CAppClient : public CClient
{
public:
	CAppClient();
	~CAppClient();

	void					DoAction();
	void					ProcessDBPack(SMysqlRespond *pRespond, SMysqlDataHead *pDataHead);
private:
	void					ProcessNetPack();

	void					DefaultProtocolFunc(const void *pPack, const unsigned int uPackLen);

	void					RecvLogin(const void *pPack, const unsigned int uPackLen);
	void					RecvRequestSlopeList(const void *pPack, const unsigned int uPackLen);
	void					RecvRequestSensorList(const void *pPack, const unsigned int uPackLen);
	void					RecvRequestSensorHistory(const void *pPack, const unsigned int uPackLen);
private:
	typedef void			(CAppClient::*pfnProtocolFunc)(const void *pPack, const unsigned int uPackLen);
	static pfnProtocolFunc	m_ProtocolFunc[APP_SERVER_NET_Protocol::APP2S::app2s_max];
private:
};

#endif
