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

void CWebConnection::ResetTimeOut()
{
	m_nTimeOut	= g_nTimeNow + g_pCenterServerConfig.m_nWebTimeOut;
}

void CWebConnection::Disconnect()
{
	g_ICenterServerLogic.WebLogout(this);

	m_eState	= CLIENT_CONN_STATE_IDLE;

	if (m_pTcpConnection)
	{
		// ���������̵߳Ļص�������m_pTcpConnectionʱ������Ĵ����ֽ�m_pTcpConnection��Ϊ��nullptr
		ITcpConnection	*pTcpConnection	= m_pTcpConnection;
		m_pTcpConnection	= nullptr;

		pTcpConnection->ShutDown();
	}
}

void CWebConnection::OnWaitLogin()
{
	if (!g_ICenterServerLogic.WebLogin(this))
	{
		Disconnect();
		return;
	}

	m_eState	= CLIENT_CONN_STATE_RUNNING;
}
