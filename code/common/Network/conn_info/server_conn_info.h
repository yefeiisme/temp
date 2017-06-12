#ifndef __SERVER_CONN_INFO_H_
#define __SERVER_CONN_INFO_H_

#include "conn_info.h"

class CServerConnInfo : public CTcpConnection
{
	enum E_SERVER_CONN_STATE
	{
		SERVER_CONN_IDLE,
		SERVER_CONN_CONNECT,
		SERVER_CONN_WAIT_LOGIC_EXIT,
		SERVER_CONN_STATE_MAX,
	};
private:
	typedef void				(CServerConnInfo::*pfnStateFunc)();
	static pfnStateFunc			m_pfnStateFunc[SERVER_CONN_STATE_MAX];

	E_SERVER_CONN_STATE			m_eState;
public:
	CServerConnInfo();
	~CServerConnInfo();

	const char					*GetIP();
	inline void					Connected()
	{
		CTcpConnection::Connected();

		m_eState = SERVER_CONN_CONNECT;
	}
private:
	void						OnIdle();
	void						OnConnect();
	void						OnWaitLogicExit();
};

#endif
