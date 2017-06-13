#ifndef __SERVER_CONN_INFO_H_
#define __SERVER_CONN_INFO_H_

#include "conn_info.h"

enum E_SERVER_CONN_STATE
{
	SERVER_CONN_IDLE,
	SERVER_CONN_CONNECT,
	SERVER_CONN_WAIT_LOGIC_EXIT,
	SERVER_CONN_STATE_MAX,
};

class CServerConnInfo : public CTcpConnection
{
private:
	E_SERVER_CONN_STATE			m_eState;
public:
	CServerConnInfo();
	~CServerConnInfo();

	inline E_SERVER_CONN_STATE	GetState()
	{
		return m_eState;
	}

	const char					*GetIP();
	inline void					Connected()
	{
		CTcpConnection::Connected();

		m_eState = SERVER_CONN_CONNECT;
	}

	inline void					Disconnect()
	{
		CTcpConnection::Disconnect();

		m_eState = SERVER_CONN_WAIT_LOGIC_EXIT;
	}

	inline void					LogicDisconnect()
	{
		m_eState = SERVER_CONN_IDLE;
	}
};

#endif
