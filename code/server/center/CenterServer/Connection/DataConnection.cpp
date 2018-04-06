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
// 功能：网络库中，当有传感器连接上来后，由框架类调用此函数来设置连接的状态
//
void CDataConnection::Connect(ITcpConnection *pTcpConnection)
{
	m_pTcpConnection	= pTcpConnection;

	m_nTimeOut			= g_nTimeNow + g_pCenterServerConfig.m_nDataTimeOut;

	m_eState			= CLIENT_CONN_STATE_WAIT_LOGIN;
}

//=====================================================
// 功能：重置连接超时的时间，用于保持连接
//
void CDataConnection::ResetTimeOut()
{
	m_nTimeOut = g_nTimeNow + g_pCenterServerConfig.m_nDataTimeOut;
}

//=====================================================
// 功能：用于逻辑层关闭网络连接的调用
//
void CDataConnection::Disconnect()
{
	g_ICenterServerLogic.DataClientLogout(this);

	m_eState = CLIENT_CONN_STATE_IDLE;

	if (m_pTcpConnection)
	{
		// 以免网络线程的回调在设置m_pTcpConnection时，这里的代码又将m_pTcpConnection设为了nullptr
		ITcpConnection	*pTcpConnection = m_pTcpConnection;
		m_pTcpConnection = nullptr;

		pTcpConnection->ShutDown();
	}
}

//=====================================================
// 功能：成功建立连接后状态机切到此函数。逻辑层将本对象与逻辑层中的实例进行绑定并进行相关的初始化
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
