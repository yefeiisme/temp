#ifndef __APP_CLIENT_H_
#define __APP_CLIENT_H_

#include "Client.h"
#include "App_Server_Protocol.pb.h"

class CAppClient : public CClient
{
public:
	CAppClient();
	~CAppClient();

	void					DoAction();
private:
	void					ProcessNetPack();

	void					DefaultProtocolFunc(const void *pPack, const unsigned int uPackLen);

	void					RecvPing(const void *pPack, const unsigned int uPackLen);
private:
	typedef void			(CAppClient::*pfnProtocolFunc)(const void *pPack, const unsigned int uPackLen);
	static pfnProtocolFunc	m_ProtocolFunc[APP_SERVER_NET_Protocol::APP2S::app2s_max];
private:
};

#endif
