#include "stdafx.h"
#include "SimulatorLogic.h"
#include "ServerConnection/AppServerConnection.h"
#include "ServerConnection/WebServerConnection.h"

void CSimulatorLogic::ProcessRequest()
{
	const void		*pPack		= nullptr;
	unsigned int	uPackLen	= 0;

	while (nullptr != (pPack = m_pRBRequest->RcvPack(uPackLen)))
	{
		BYTE byProtocol	= *((BYTE*)pPack);

		(this->*m_ProtocolFunc[byProtocol])(pPack, uPackLen);
	};
}

void CSimulatorLogic::RecvAppLogin(const void *pPack, const unsigned int uPackLen)
{
	U2L_APP_LOGIN	*pUIRequest	= (U2L_APP_LOGIN*)pPack;

	char	strBuffer[0xff];
	BYTE	*pProtocol	= (BYTE*)strBuffer;
	*pProtocol	= APP_SERVER_NET_Protocol::APP2S::app2s_login;

	APP_SERVER_NET_Protocol::App2S_Login	tagLogin;
	tagLogin.set_account(pUIRequest->strAccount);
	tagLogin.set_password(pUIRequest->strPassword);

	if (tagLogin.ByteSize() > sizeof(strBuffer) - sizeof(BYTE))
		return;

	tagLogin.SerializePartialToArray(strBuffer + sizeof(BYTE), tagLogin.ByteSize());

	m_pAppServerConnList[0].PutPack(strBuffer, sizeof(BYTE) + tagLogin.ByteSize());
}

void CSimulatorLogic::RecvWebLogin(const void *pPack, const unsigned int uPackLen)
{
	U2L_WEB_LOGIN	*pUIRequest	= (U2L_WEB_LOGIN*)pPack;

	char	strBuffer[0xff];
	BYTE	*pProtocol	= (BYTE*)strBuffer;
	*pProtocol	= WEB_SERVER_NET_Protocol::WEB2S::web2s_login;

	WEB_SERVER_NET_Protocol::Web2S_Login	tagLogin;
	tagLogin.set_account(pUIRequest->strAccount);
	tagLogin.set_password(pUIRequest->strPassword);

	if (tagLogin.ByteSize() > sizeof(strBuffer)-sizeof(BYTE))
		return;

	tagLogin.SerializePartialToArray(strBuffer + sizeof(BYTE), tagLogin.ByteSize());

	m_pWebServerConnList[0].PutPack(strBuffer, sizeof(BYTE)+tagLogin.ByteSize());
}
