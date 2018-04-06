#include "stdafx.h"
#include "ClientConnection.h"
#include "CenterServer.h"
//#include "IGameServerLogic.h"

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

	m_nTimeOut			= 0;
}

CClientConnection::~CClientConnection()
{
	if (m_pTcpConnection)
	{
		m_pTcpConnection->ShutDown();
		m_pTcpConnection	= nullptr;
	}
}

//=====================================================
// ���ܣ��������ӵ�״̬��������m_pTcpConnection->IsConnect��������ȡ����������״̬���Ծ����Ƿ�Ҫ����ִ��״̬���������ǶϿ���������
//
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

//=====================================================
// ���ܣ���ȡ���������װ������ģ�����
//
const void *CClientConnection::GetPack(unsigned int &uPackLen)
{
	if (nullptr == m_pTcpConnection)
		return nullptr;

	return m_pTcpConnection->GetPack(uPackLen);
}

//=====================================================
// ���ܣ��������������װ������ģ�����
//
bool CClientConnection::PutPack(const void *pPack, unsigned int uPackLen)
{
	if (nullptr == m_pTcpConnection)
		return false;

	return m_pTcpConnection->PutPack(pPack, uPackLen);
}

//=====================================================
// ���ܣ������������ӵĳ�ʱʱ�䡣�麯����ÿ���ࣨ�����������͵��ࣩ�Լ�ʵ�־������ݡ�
//
void CClientConnection::ResetTimeOut()
{
	m_nTimeOut	= g_nTimeNow + 60;
}

//=====================================================
// ���ܣ�״̬����������ʱ��״̬����
//
void CClientConnection::OnIdle()
{
}

//=====================================================
// ���ܣ�״̬��������������ʱ���ȴ��������룬����֤�˺����롣�麯����ÿ���ࣨ�����������͵��ࣩ�Լ�ʵ�־������ݡ�
//
void CClientConnection::OnWaitLogin()
{
}

//=====================================================
// ���ܣ�״̬����һ�ж�OK���л�����״̬����������Ϣ�����ݿ���Ϣ������������״ֻ̬����ʱ��Ĺ���
//
void CClientConnection::OnRunning()
{
	if (IsTimeOut())
	{
		Disconnect();
		return;
	}
}

//=====================================================
// ���ܣ��������߱����Ͽ�����ʱ�����ô˺������麯����ÿ�������Լ�ʵ�־������ݡ�
//
void CClientConnection::Disconnect()
{
}
