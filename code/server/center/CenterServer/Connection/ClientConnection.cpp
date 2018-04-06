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
// 功能：网络连接的状态机。其中m_pTcpConnection->IsConnect是用来获取网络层的连接状态，以决定是否要继续执行状态机，或者是断开网络连接
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
// 功能：获取网络包。封装给其它模块调用
//
const void *CClientConnection::GetPack(unsigned int &uPackLen)
{
	if (nullptr == m_pTcpConnection)
		return nullptr;

	return m_pTcpConnection->GetPack(uPackLen);
}

//=====================================================
// 功能：发送网络包。封装给其它模块调用
//
bool CClientConnection::PutPack(const void *pPack, unsigned int uPackLen)
{
	if (nullptr == m_pTcpConnection)
		return false;

	return m_pTcpConnection->PutPack(pPack, uPackLen);
}

//=====================================================
// 功能：重置网络连接的超时时间。虚函数，每子类（具体连接类型的类）自己实现具体内容。
//
void CClientConnection::ResetTimeOut()
{
	m_nTimeOut	= g_nTimeNow + 60;
}

//=====================================================
// 功能：状态机。无连接时的状态处理
//
void CClientConnection::OnIdle()
{
}

//=====================================================
// 功能：状态机。刚连接上来时，等待连接输入，以验证账号密码。虚函数，每子类（具体连接类型的类）自己实现具体内容。
//
void CClientConnection::OnWaitLogin()
{
}

//=====================================================
// 功能：状态机。一切都OK后，切换至此状态。由网络消息和数据库消息进行驱动。本状态只处理超时后的功能
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
// 功能：主动或者被动断开连接时，调用此函数。虚函数，每种连接自己实现具体内容。
//
void CClientConnection::Disconnect()
{
}
