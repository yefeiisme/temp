#include "stdafx.h"
#include "ClientConnection.h"
#include "GameServer.h"
#include "IGameServerLogic.h"

CClientConnection::StateFuncArray CClientConnection::m_pfnStateFunc[CLIENT_CONN_STATE_MAX] =
{
	&CClientConnection::OnIdle,
	&CClientConnection::OnWaitLogin,
	&CClientConnection::OnRunning,
};

CClientConnection::CClientConnection()
{
	m_pTcpConnection	= nullptr;

	m_eState			= CLIENT_CONN_STATE_IDLE;
}

CClientConnection::~CClientConnection()
{
	if (m_pTcpConnection)
	{
		m_pTcpConnection->ShutDown();
		m_pTcpConnection	= nullptr;
	}
}

void CClientConnection::DoAction()
{
	if (m_pTcpConnection->IsConnect())
	{
		(this->*CClientConnection::m_pfnStateFunc[m_eState])();
	}
	else
	{
		Disconnect();
	}
}

const void *CClientConnection::GetPack(unsigned int &uPackLen)
{
	if (nullptr == m_pTcpConnection)
		return nullptr;

	return m_pTcpConnection->GetPack(uPackLen);
}

bool CClientConnection::PutPack(const void *pPack, unsigned int uPackLen)
{
	if (nullptr == m_pTcpConnection)
		return false;

	return m_pTcpConnection->PutPack(pPack, uPackLen);
}

void CClientConnection::ResetTimeOut()
{
	m_nTimeOut	= g_nGameServerSecond + 10;
}

void CClientConnection::OnIdle()
{
}

void CClientConnection::OnWaitLogin()
{
	if (!g_IGameServerLogic.ClientLogin(this))
	{
		Disconnect();
		return;
	}

	m_eState	= CLIENT_CONN_STATE_RUNNING;
}

void CClientConnection::OnRunning()
{
	//if (IsTimeOut())
	//{
	//	Disconnect();
	//	return;
	//}
}

void CClientConnection::Disconnect()
{
	if (m_pTcpConnection)
	{
		m_pTcpConnection->ShutDown();
		m_pTcpConnection	= nullptr;
	}

	g_IGameServerLogic.ClientLogout(this);
	m_eState	= CLIENT_CONN_STATE_IDLE;
}
