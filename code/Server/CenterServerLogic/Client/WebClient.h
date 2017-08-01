#ifndef __WEB_CLIENT_H_
#define __WEB_CLIENT_H_

#include "Client.h"

class CWebClient : public CClient
{
public:
	CWebClient();
	~CWebClient();

	void					DoAction();
private:
	void					ProcessNetPack();

	void					RecvPing(const void *pPack, const unsigned int uPackLen);
private:
	typedef void			(CWebClient::*pfnProtocolFunc)(const void *pPack, const unsigned int uPackLen);
	static pfnProtocolFunc	m_ProtocolFunc[256];
private:
};

#endif
