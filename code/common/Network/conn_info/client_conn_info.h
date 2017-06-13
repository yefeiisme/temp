#ifndef __CLIENT_CONN_INFO_H_
#define __CLIENT_CONN_INFO_H_

#include "conn_info.h"

enum E_CLIENT_CONN_STATE
{
	CLIENT_CONN_IDLE,
	CLIENT_CONN_TRY_CONNECT,
	CLIENT_CONN_WAIT_CONNECT,
	CLIENT_CONN_CONNECTED,
	CLIENT_CONN_WAIT_LOGIC_EXIT,
	CLIENT_CONN_STATE_MAX,
};

class CClientConnInfo : public CTcpConnection
{
private:
	E_CLIENT_CONN_STATE			m_eState;
	unsigned int				m_uTargetIndex;					// 用于连接成功后的回调对象（只做为客户端对象连接服务器端时有效）

	unsigned short				m_usPort;						// 连接的端口
	char						m_strConnectToIP[MAX_IP_LEN];	// 连接的IP地址
public:
	CClientConnInfo();
	~CClientConnInfo(); 

	inline E_CLIENT_CONN_STATE	GetState() const
	{
		return m_eState;
	}

	inline void					SetConnectTarget(const unsigned int uIndex)
	{
		m_uTargetIndex = uIndex;
	}

	inline unsigned int			GetConnectTarget() const
	{
		return m_uTargetIndex;
	}

	inline unsigned short		GetConnectToPort() const
	{
		return m_usPort;
	}

	inline const char			*GetConnectToIP()
	{
		return m_strConnectToIP;
	}

	inline void					ConnectTo(char *pstrIP, const unsigned short usPort)
	{
		strncpy(m_strConnectToIP, pstrIP, sizeof(m_strConnectToIP));
		m_strConnectToIP[sizeof(m_strConnectToIP)-1] = '\0';

		m_usPort			= usPort;

		m_bLogicConnected	= true;

		m_eState			= CLIENT_CONN_WAIT_CONNECT;
	}

	inline void					TryConnect()
	{
		m_eState = CLIENT_CONN_TRY_CONNECT;
	}

	inline void					WaitConnectOK()
	{
		m_eState = CLIENT_CONN_WAIT_CONNECT;
	}

	inline const char			*GetIP()
	{
		return m_strConnectToIP;
	}

	inline void					Connected()
	{
		CTcpConnection::Connected();

		m_eState = CLIENT_CONN_CONNECTED;
	}

	void						Disconnect();

	void						LogicDisconnect();
};

#endif
