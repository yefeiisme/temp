#include "stdafx.h"
#include "WebConnection.h"
#include "ICenterServerLogic.h"
#include "../Config/CenterServerConfig.h"

CWebConnection::CWebConnection()
{
}

CWebConnection::~CWebConnection()
{
}

void CWebConnection::Connect(ITcpConnection *pTcpConnection)
{
	m_pTcpConnection	= pTcpConnection;

	m_nTimeOut			= g_nTimeNow + g_pCenterServerConfig.m_nWebTimeOut;

	m_eState			= CLIENT_CONN_STATE_WAIT_LOGIN;
}

void CWebConnection::ResetTimeOut()
{
	m_nTimeOut	= g_nTimeNow + g_pCenterServerConfig.m_nWebTimeOut;
}

void CWebConnection::Disconnect()
{
	g_ICenterServerLogic.WebClientLogout(this);

	m_eState	= CLIENT_CONN_STATE_IDLE;

	if (m_pTcpConnection)
	{
		// 以免网络线程的回调在设置m_pTcpConnection时，这里的代码又将m_pTcpConnection设为了nullptr
		ITcpConnection	*pTcpConnection	= m_pTcpConnection;
		m_pTcpConnection	= nullptr;

		pTcpConnection->ShutDown();
	}
}

void CWebConnection::OnWaitLogin()
{
	if (!g_ICenterServerLogic.WebClientLogin(this))
	{
		Disconnect();
		return;
	}

	ChangeState(CLIENT_CONN_STATE_RUNNING, g_pCenterServerConfig.m_nWebTimeOut);
}
