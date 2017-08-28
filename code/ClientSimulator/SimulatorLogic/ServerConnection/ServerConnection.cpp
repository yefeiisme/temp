#include "stdafx.h"
#include "ServerConnection.h"
#include "SimulatorLogic.h"

CServerConnection::StateFuncArray CServerConnection::m_pfnStateFunc[SERVER_CONN_STATE_MAX] =
{
	&CServerConnection::OnIdle,
	&CServerConnection::OnWaitConnect,
	&CServerConnection::OnRunning,
	&CServerConnection::OnDisconnect,
};

CServerConnection::CServerConnection()
{
	m_pTcpConnection	= nullptr;

	m_uIndex			= 0;
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
	(this->*CServerConnection::m_pfnStateFunc[m_eState])();
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
	if (IsTimeOut())
	{
		ChangeState(SERVER_CONN_STATE_DISCONNECT);
		return;
	}

	if (nullptr == m_pTcpConnection)
		return;

	if (!m_pTcpConnection->IsConnect())
		return;

	ChangeState(SERVER_CONN_STATE_RUNNING);
}

void CServerConnection::OnRunning()
{
	if (IsTimeOut())
	{
		ChangeState(SERVER_CONN_STATE_DISCONNECT);
		return;
	}

	if (!m_pTcpConnection->IsConnect())
	{
		ChangeState(SERVER_CONN_STATE_DISCONNECT);
		return;
	}

	const void		*pPack		= nullptr;
	unsigned int	uPackLen	= 0;

	while (nullptr != (pPack = GetPack(uPackLen)))
	{
		ProcessNetPack(pPack, uPackLen);

		ResetTimeOut();
	};
}

void CServerConnection::OnDisconnect()
{
	if (m_pTcpConnection)
	{
		m_pTcpConnection->ShutDown();
		m_pTcpConnection	= nullptr;
	}
	else
	{
		g_pSimulatorLogic.ShutDownConnection(m_uIndex);
	}

	ChangeState(SERVER_CONN_STATE_IDLE);
}
