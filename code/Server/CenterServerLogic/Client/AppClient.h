#ifndef __APP_CLIENT_H_
#define __APP_CLIENT_H_

#include "Client.h"

class CAppClient : public CClient
{
public:
	CAppClient();
	~CAppClient();

	void					DoAction();
private:
	void					ProcessNetPack();

	void					RecvPing(const void *pPack, const unsigned int uPackLen);
private:
	typedef void			(CAppClient::*pfnProtocolFunc)(const void *pPack, const unsigned int uPackLen);
	static pfnProtocolFunc	m_ProtocolFunc[256];
private:
};

#endif
