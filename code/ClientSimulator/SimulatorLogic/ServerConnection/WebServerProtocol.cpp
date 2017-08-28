#include "stdafx.h"
#include "WebServerConnection.h"

CWebServerConnection::pfnProtocolFunc CWebServerConnection::m_ProtocolFunc[WEB_SERVER_NET_Protocol::S2WEB::s2web_max] =
{
	&CWebServerConnection::RecvLoginResult
};

void CWebServerConnection::ProcessNetPack(const void *pPack, const unsigned int uPackLen)
{
	if (nullptr == pPack)
		return;

	BYTE			byProtocol	= 0;

	byProtocol	= *((BYTE*)pPack);

	if (byProtocol >= WEB_SERVER_NET_Protocol::S2WEB::s2web_max)
	{
		g_pFileLog->WriteLog("[%s][%d] Server Connection Invalid Protocol[%hhu]\n", __FILE__, __LINE__, byProtocol);
		return;
	}

	(this->*m_ProtocolFunc[byProtocol])(pPack, uPackLen);
}

void CWebServerConnection::RecvLoginResult(const void *pPack, const unsigned int uPackLen)
{
}
