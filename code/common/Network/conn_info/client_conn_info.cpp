#include "client_conn_info.h"

CClientConnInfo::CClientConnInfo() : CTcpConnection()
{
	m_eState		= CLIENT_CONN_IDLE;
	m_uTargetIndex	= 0xffffffff;
	m_usPort		= 0;
	memset(m_strConnectToIP, 0, sizeof(m_strConnectToIP));
}

CClientConnInfo::~CClientConnInfo()
{
	// ������Ҫȷ��һ�£��Ƿ����ø������������
	// ...
}

void CClientConnInfo::Disconnect()
{
	CTcpConnection::Disconnect();

	m_eState = CLIENT_CONN_WAIT_LOGIC_EXIT;
}

void CClientConnInfo::LogicDisconnect()
{
	m_uTargetIndex	= 0xffffffff;
	m_usPort		= 0;
	memset(m_strConnectToIP, 0, sizeof(m_strConnectToIP));
	m_eState		= CLIENT_CONN_IDLE;
}
