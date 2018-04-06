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

//=====================================================
// ���ܣ�������У�����app�����������ɿ������ô˺������������ӵ�״̬
//
void CAppConnection::Connect(ITcpConnection *pTcpConnection)
{
	m_pTcpConnection	= pTcpConnection;

	m_nTimeOut			= g_nTimeNow + g_pCenterServerConfig.m_nAppTimeOut;

	m_eState			= CLIENT_CONN_STATE_WAIT_LOGIN;
}

//=====================================================
// ���ܣ��������ӳ�ʱ��ʱ�䣬���ڱ�������
//
void CAppConnection::ResetTimeOut()
{
	m_nTimeOut	= g_nTimeNow + g_pCenterServerConfig.m_nAppTimeOut;
}

//=====================================================
// ���ܣ������߼���ر��������ӵĵ���
//
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

//=====================================================
// ���ܣ��ɹ��������Ӻ�״̬���е��˺������߼��㽫���������߼����е�ʵ�����а󶨲�������صĳ�ʼ��
//
void CAppConnection::OnWaitLogin()
{
	if (!g_ICenterServerLogic.AppClientLogin(this))
	{
		Disconnect();
		return;
	}

	ChangeState(CLIENT_CONN_STATE_RUNNING, g_pCenterServerConfig.m_nAppTimeOut);
}
