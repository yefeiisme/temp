#ifndef __WEB_CLIENT_H_
#define __WEB_CLIENT_H_

#include "Client.h"
#include "Web_Server_Protocol.pb.h"

class CWebClient : public CClient
{
public:
	CWebClient();
	~CWebClient();

	void					DoAction();
private:
	void					ProcessNetPack();

	void					DefaultProtocolFunc(const void *pPack, const unsigned int uPackLen);

	void					RecvLogin(const void *pPack, const unsigned int uPackLen);
	void					RecvRequestSlopeList(const void *pPack, const unsigned int uPackLen);
	void					RecvRequestSensorList(const void *pPack, const unsigned int uPackLen);
	void					RecvRequestSensorHistory(const void *pPack, const unsigned int uPackLen);
private:
	typedef void			(CWebClient::*pfnProtocolFunc)(const void *pPack, const unsigned int uPackLen);
	static pfnProtocolFunc	m_ProtocolFunc[WEB_SERVER_NET_Protocol::WEB2S::web2s_max];
private:
};

#endif
