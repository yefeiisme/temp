#include "stdafx.h"
#include "AppConnection.h"
#include "ICenterServerLogic.h"
#include "../Config/CenterServerConfig.h"

CAppConnection::CAppConnection()
{
}

CAppConnection::~CAppConnection()
{
}

void CAppConnection::ResetTimeOut()
{
	m_nTimeOut	= g_nTimeNow + g_pCenterServerConfig.m_nAppTimeOut;
}

void CAppConnection::Disconnect()
{
	g_ICenterServerLogic.AppClientLogout(this);

	m_eState	= CLIENT_CONN_STATE_IDLE;

	if (m_pTcpConnection)
	{
		// ���������̵߳Ļص�������m_pTcpConnectionʱ������Ĵ����ֽ�m_pTcpConnection��Ϊ��nullptr
		ITcpConnection	*pTcpConnection	= m_pTcpConnection;
		m_pTcpConnection	= nullptr;

		pTcpConnection->ShutDown();
	}
}

void CAppConnection::OnWaitLogin()
{
	if (!g_ICenterServerLogic.AppClientLogin(this))
	{
		Disconnect();
		return;
	}

	ChangeState(CLIENT_CONN_STATE_RUNNING, g_pCenterServerConfig.m_nAppTimeOut);
}
