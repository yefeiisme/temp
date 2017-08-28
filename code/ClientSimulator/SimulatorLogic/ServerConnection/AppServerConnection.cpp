#include "stdafx.h"
#include "AppServerConnection.h"
#include "SimulatorLogic.h"

CAppServerConnection::StateFuncArray CAppServerConnection::m_pfnStateFunc[SERVER_CONN_STATE_MAX] =
{
	&CAppServerConnection::OnIdle,
	&CAppServerConnection::OnWaitConnect,
	&CAppServerConnection::OnRunning,
	&CAppServerConnection::OnDisconnect,
};

CAppServerConnection::CAppServerConnection()
{
	m_pTcpConnection	= nullptr;

	m_uIndex			= 0;
	m_eState			= SERVER_CONN_STATE_IDLE;
	m_nTimeOut			= 0;
}

CAppServerConnection::~CAppServerConnection()
{
	if (m_pTcpConnection)
	{
		m_pTcpConnection->ShutDown();
		m_pTcpConnection	= nullptr;
	}
}

void CAppServerConnection::DoAction()
{
	(this->*CAppServerConnection::m_pfnStateFunc[m_eState])();
}

const void *CAppServerConnection::GetPack(unsigned int &uPackLen)
{
	if (nullptr == m_pTcpConnection)
		return nullptr;

	return m_pTcpConnection->GetPack(uPackLen);
}

bool CAppServerConnection::PutPack(const void *pPack, unsigned int uPackLen)
{
	if (nullptr == m_pTcpConnection)
		return false;

	return m_pTcpConnection->PutPack(pPack, uPackLen);
}

void CAppServerConnection::ResetTimeOut()
{
	m_nTimeOut	= g_nSimulatorSecond + 10;
}

void CAppServerConnection::OnIdle()
{
}

void CAppServerConnection::OnWaitConnect()
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

void CAppServerConnection::OnRunning()
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

void CAppServerConnection::OnDisconnect()
{
	if (m_pTcpConnection)
	{
		m_pTcpConnection->ShutDown();
		m_pTcpConnection	= nullptr;
	}
	else
	{
		g_pSimulatorLogic.ShutDownAppServerConnection(m_uIndex);
	}

	ChangeState(SERVER_CONN_STATE_IDLE);
}
