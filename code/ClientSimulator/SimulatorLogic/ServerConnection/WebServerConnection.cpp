#include "stdafx.h"
#include "WebServerConnection.h"
#include "SimulatorLogic.h"

CWebServerConnection::StateFuncArray CWebServerConnection::m_pfnStateFunc[WEB_SERVER_CONN_STATE_MAX] =
{
	&CWebServerConnection::OnIdle,
	&CWebServerConnection::OnWaitConnect,
	&CWebServerConnection::OnRunning,
	&CWebServerConnection::OnDisconnect,
};

CWebServerConnection::CWebServerConnection()
{
	m_pTcpConnection	= nullptr;

	m_uIndex			= 0;
	m_eState			= WEB_SERVER_CONN_STATE_IDLE;
	m_nTimeOut			= 0;
}

CWebServerConnection::~CWebServerConnection()
{
	if (m_pTcpConnection)
	{
		m_pTcpConnection->ShutDown();
		m_pTcpConnection	= nullptr;
	}
}

void CWebServerConnection::DoAction()
{
	(this->*CWebServerConnection::m_pfnStateFunc[m_eState])();
}

const void *CWebServerConnection::GetPack(unsigned int &uPackLen)
{
	if (nullptr == m_pTcpConnection)
		return nullptr;

	return m_pTcpConnection->GetPack(uPackLen);
}

bool CWebServerConnection::PutPack(const void *pPack, unsigned int uPackLen)
{
	if (nullptr == m_pTcpConnection)
		return false;

	return m_pTcpConnection->PutPack(pPack, uPackLen);
}

void CWebServerConnection::ResetTimeOut()
{
	m_nTimeOut	= g_nSimulatorSecond + 10;
}

void CWebServerConnection::OnIdle()
{
}

void CWebServerConnection::OnWaitConnect()
{
	if (IsTimeOut())
	{
		ChangeState(WEB_SERVER_CONN_STATE_DISCONNECT);
		return;
	}

	if (nullptr == m_pTcpConnection)
		return;

	if (!m_pTcpConnection->IsConnect())
		return;

	ChangeState(WEB_SERVER_CONN_STATE_RUNNING);
}

void CWebServerConnection::OnRunning()
{
	if (IsTimeOut())
	{
		ChangeState(WEB_SERVER_CONN_STATE_DISCONNECT);
		return;
	}

	if (!m_pTcpConnection->IsConnect())
	{
		ChangeState(WEB_SERVER_CONN_STATE_DISCONNECT);
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

void CWebServerConnection::OnDisconnect()
{
	if (m_pTcpConnection)
	{
		m_pTcpConnection->ShutDown();
		m_pTcpConnection	= nullptr;
	}
	else
	{
		g_pSimulatorLogic.ShutDownWebServerConnection(m_uIndex);
	}

	ChangeState(WEB_SERVER_CONN_STATE_IDLE);
}
