#include "stdafx.h"
#include "ServerConnection.h"
#include "SimulatorLogic.h"
#include "../Player/Player.h"

CServerConnection::StateFuncArray CServerConnection::m_pfnStateFunc[SERVER_CONN_STATE_MAX] =
{
	&CServerConnection::OnIdle,
	&CServerConnection::OnWaitConnect,
	&CServerConnection::OnRunning,
};

CServerConnection::CServerConnection()
{
	m_pTcpConnection	= nullptr;
	m_pPlayer			= nullptr;

	m_eState			= SERVER_CONN_STATE_IDLE;
	m_nTimeOut			= 0;
}

CServerConnection::~CServerConnection()
{
	if (m_pTcpConnection)
	{
		m_pTcpConnection->ShutDown();
		m_pTcpConnection	= nullptr;
	}
}

void CServerConnection::DoAction()
{
	if (m_pTcpConnection->IsConnect())
	{
		(this->*CServerConnection::m_pfnStateFunc[m_eState])();
	}
	else
	{
		Disconnect();
	}
}

const void *CServerConnection::GetPack(unsigned int &uPackLen)
{
	if (nullptr == m_pTcpConnection)
		return nullptr;

	return m_pTcpConnection->GetPack(uPackLen);
}

bool CServerConnection::PutPack(const void *pPack, unsigned int uPackLen)
{
	if (nullptr == m_pTcpConnection)
		return false;

	return m_pTcpConnection->PutPack(pPack, uPackLen);
}

void CServerConnection::ResetTimeOut()
{
	m_nTimeOut	= g_nSimulatorSecond + 10;
}

void CServerConnection::OnIdle()
{
}

void CServerConnection::OnWaitConnect()
{
}

void CServerConnection::OnRunning()
{
	const void		*pPack		= nullptr;
	unsigned int	uPackLen	= 0;

	while (nullptr != (pPack = GetPack(uPackLen)))
	{
		m_pPlayer->ProcessNetPack(pPack, uPackLen);

		ResetTimeOut();
	};

	m_pPlayer->DoAction();
}

void CServerConnection::Disconnect()
{
	if (m_pTcpConnection)
	{
		m_pTcpConnection->ShutDown();
		m_pTcpConnection	= nullptr;
	}

	ChangeState(SERVER_CONN_STATE_IDLE);
}
