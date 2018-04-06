#include "stdafx.h"
#include "DataConnection.h"
#include "ICenterServerLogic.h"
#include "../Config/CenterServerConfig.h"

CDataConnection::CDataConnection()
{
}

CDataConnection::~CDataConnection()
{
}

//=====================================================
// ���ܣ�������У����д����������������ɿ������ô˺������������ӵ�״̬
//
void CDataConnection::Connect(ITcpConnection *pTcpConnection)
{
	m_pTcpConnection	= pTcpConnection;

	m_nTimeOut			= g_nTimeNow + g_pCenterServerConfig.m_nDataTimeOut;

	m_eState			= CLIENT_CONN_STATE_WAIT_LOGIN;
}

//=====================================================
// ���ܣ��������ӳ�ʱ��ʱ�䣬���ڱ�������
//
void CDataConnection::ResetTimeOut()
{
	m_nTimeOut = g_nTimeNow + g_pCenterServerConfig.m_nDataTimeOut;
}

//=====================================================
// ���ܣ������߼���ر��������ӵĵ���
//
void CDataConnection::Disconnect()
{
	g_ICenterServerLogic.DataClientLogout(this);

	m_eState = CLIENT_CONN_STATE_IDLE;

	if (m_pTcpConnection)
	{
		// ���������̵߳Ļص�������m_pTcpConnectionʱ������Ĵ����ֽ�m_pTcpConnection��Ϊ��nullptr
		ITcpConnection	*pTcpConnection = m_pTcpConnection;
		m_pTcpConnection = nullptr;

		pTcpConnection->ShutDown();
	}
}

//=====================================================
// ���ܣ��ɹ��������Ӻ�״̬���е��˺������߼��㽫���������߼����е�ʵ�����а󶨲�������صĳ�ʼ��
//
void CDataConnection::OnWaitLogin()
{
	if (!g_ICenterServerLogic.DataClientLogin(this))
	{
		Disconnect();
		return;
	}

	ChangeState(CLIENT_CONN_STATE_RUNNING, g_pCenterServerConfig.m_nDataTimeOut);
}
