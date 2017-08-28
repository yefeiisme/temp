#include "stdafx.h"
#include "AppServerConnection.h"

CAppServerConnection::pfnProtocolFunc CAppServerConnection::m_ProtocolFunc[APP_SERVER_NET_Protocol::S2APP::s2app_max] =
{
	&CAppServerConnection::RecvLoginResult
};

void CAppServerConnection::ProcessNetPack(const void *pPack, const unsigned int uPackLen)
{
	if (nullptr == pPack)
		return;

	BYTE			byProtocol	= 0;

	byProtocol	= *((BYTE*)pPack);

	if (byProtocol >= APP_SERVER_NET_Protocol::S2APP::s2app_max)
	{
		g_pFileLog->WriteLog("[%s][%d] Server Connection Invalid Protocol[%hhu]\n", __FILE__, __LINE__, byProtocol);
		return;
	}

	(this->*m_ProtocolFunc[byProtocol])(pPack, uPackLen);
}

void CAppServerConnection::RecvLoginResult(const void *pPack, const unsigned int uPackLen)
{
}
