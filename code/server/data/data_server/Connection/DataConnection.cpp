#include "stdafx.h"
#include "DataConnection.h"
#include "IDataLogic.h"
#include "../Config/DataServerConfig.h"

CDataConnection::CDataConnection()
{
}

CDataConnection::~CDataConnection()
{
}

void CDataConnection::ResetTimeOut()
{
	m_nTimeOut = g_nTimeNow + g_pDataServerConfig.m_nDataTimeOut;
}

void CDataConnection::Disconnect()
{
	g_IDataLogic.DataClientLogout(this);

	m_eState = CLIENT_CONN_STATE_IDLE;

	if (m_pTcpConnection)
	{
		// ���������̵߳Ļص�������m_pTcpConnectionʱ������Ĵ����ֽ�m_pTcpConnection��Ϊ��nullptr
		ITcpConnection	*pTcpConnection = m_pTcpConnection;
		m_pTcpConnection = nullptr;

		pTcpConnection->ShutDown();
	}
}

void CDataConnection::OnWaitLogin()
{
	if (!g_IDataLogic.DataClientLogin(this))
	{
		Disconnect();
		return;
	}

	m_eState = CLIENT_CONN_STATE_RUNNING;
}
