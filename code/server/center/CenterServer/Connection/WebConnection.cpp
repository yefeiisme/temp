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

//=====================================================
// ���ܣ�������У����д����������������ɿ������ô˺������������ӵ�״̬
//
void CWebConnection::Connect(ITcpConnection *pTcpConnection)
{
	m_pTcpConnection	= pTcpConnection;

	m_nTimeOut			= g_nTimeNow + g_pCenterServerConfig.m_nWebTimeOut;

	m_eState			= CLIENT_CONN_STATE_WAIT_LOGIN;
}

//=====================================================
// ���ܣ��������ӳ�ʱ��ʱ�䣬���ڱ�������
//
void CWebConnection::ResetTimeOut()
{
	m_nTimeOut	= g_nTimeNow + g_pCenterServerConfig.m_nWebTimeOut;
}

//=====================================================
// ���ܣ������߼���ر��������ӵĵ���
//
void CWebConnection::Disconnect()
{
	g_ICenterServerLogic.WebClientLogout(this);

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
void CWebConnection::OnWaitLogin()
{
	if (!g_ICenterServerLogic.WebClientLogin(this))
	{
		Disconnect();
		return;
	}

	ChangeState(CLIENT_CONN_STATE_RUNNING, g_pCenterServerConfig.m_nWebTimeOut);
}
