#include "stdafx.h"
#include "AppClient.h"

CAppClient::pfnProtocolFunc CAppClient::m_ProtocolFunc[APP_SERVER_NET_Protocol::APP2S::app2s_max] =
{
	&CAppClient::RecvLogin,
	&CAppClient::RecvRequestSlopeList,
	&CAppClient::RecvRequestSensorList,
	&CAppClient::RecvRequestSensorHistory,
	&CAppClient::DefaultProtocolFunc,

	&CAppClient::DefaultProtocolFunc,
	&CAppClient::DefaultProtocolFunc,
	&CAppClient::DefaultProtocolFunc,
	&CAppClient::DefaultProtocolFunc,
	&CAppClient::DefaultProtocolFunc,

	&CAppClient::DefaultProtocolFunc,
	&CAppClient::DefaultProtocolFunc,
	&CAppClient::DefaultProtocolFunc,
	&CAppClient::DefaultProtocolFunc,
	&CAppClient::DefaultProtocolFunc,

	&CAppClient::DefaultProtocolFunc,
	&CAppClient::DefaultProtocolFunc,
	&CAppClient::DefaultProtocolFunc,
	&CAppClient::DefaultProtocolFunc,
	&CAppClient::DefaultProtocolFunc,

	&CAppClient::DefaultProtocolFunc,
	&CAppClient::DefaultProtocolFunc,
	&CAppClient::DefaultProtocolFunc,
	&CAppClient::DefaultProtocolFunc,
	&CAppClient::DefaultProtocolFunc,

};

CAppClient::CAppClient() : CClient()
{
}

CAppClient::~CAppClient()
{
}

void CAppClient::DoAction()
{
	ProcessNetPack();
}

void CAppClient::ProcessNetPack()
{
	const void		*pPack		= nullptr;
	unsigned int	uPackLen	= 0;
	BYTE			byProtocol	= 0;

	while (nullptr != (pPack = m_pClientConn->GetPack(uPackLen)))
	{
		byProtocol	= *((BYTE*)pPack);

		if (byProtocol >= 256)
		{
			g_pFileLog->WriteLog("[%s][%d] App Client[%u] Invalid Protocol[%hhu]\n", __FUNCTION__, __LINE__, m_uUniqueID, byProtocol);
			return;
		}

		(this->*m_ProtocolFunc[byProtocol])(pPack, uPackLen);

		m_pClientConn->ResetTimeOut();
	};
}

void CAppClient::DefaultProtocolFunc(const void *pPack, const unsigned int uPackLen)
{
}

void CAppClient::RecvLogin(const void *pPack, const unsigned int uPackLen)
{
}

void CAppClient::RecvRequestSlopeList(const void *pPack, const unsigned int uPackLen)
{
}

void CAppClient::RecvRequestSensorList(const void *pPack, const unsigned int uPackLen)
{
}

void CAppClient::RecvRequestSensorHistory(const void *pPack, const unsigned int uPackLen)
{
}
