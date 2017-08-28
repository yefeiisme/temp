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
		m_pAppServerConnList[0].PutPack(pPack, uPackLen);
	};
}

void CSimulatorLogic::RecvAppLogin(const void *pPack, const unsigned int uPackLen)
{
}

void CSimulatorLogic::RecvWebLogin(const void *pPack, const unsigned int uPackLen)
{
}
