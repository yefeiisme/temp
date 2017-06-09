#ifndef __CLIENT_CONNECTION_H_
#define __CLIENT_CONNECTION_H_

#include "network_interface.h"
#include "ISimulatorLogic.h"

class CPlayer;

class CServerConnection
{
	enum E_SERVER_CONN_STATE
	{
		SERVER_CONN_STATE_IDLE,
		SERVER_CONN_STATE_WAIT_CONNECT,
		SERVER_CONN_STATE_RUNNING,
		SERVER_CONN_STATE_MAX,
	};
public:
	CServerConnection();
	~CServerConnection();

	inline void				Connect(ITcpConnection &pTcpConnection)
	{
		m_pTcpConnection	= &pTcpConnection;

		m_eState	= SERVER_CONN_STATE_RUNNING;

		m_nTimeOut	= g_nSimulatorSecond + 10;
	}

	void					DoAction();

	const void				*GetPack(unsigned int &uPackLen);
	bool					PutPack(const void *pPack, unsigned int uPackLen);
	void					ResetTimeOut();

	inline void				ConnectWait()
	{
		m_eState	= SERVER_CONN_STATE_WAIT_CONNECT;
	}

	inline bool				IsIdle()
	{
		return SERVER_CONN_STATE_IDLE == m_eState;
	}

	inline bool				IsConnectWait()
	{
		return SERVER_CONN_STATE_WAIT_CONNECT == m_eState;
	}

	inline bool				IsTimeOut()
	{
		return g_nSimulatorSecond > m_nTimeOut;
	}
private:
	inline void				ChangeState(const E_SERVER_CONN_STATE eState, const int nTimeOut = 10)
	{
		m_nTimeOut	= g_nSimulatorSecond + nTimeOut;
		m_eState	= eState;
	}

	void					OnIdle();
	void					OnWaitConnect();
	void					OnRunning();
public:
	void					Disconnect();
private:
	typedef void (CServerConnection::*StateFuncArray)();
	static StateFuncArray	m_pfnStateFunc[SERVER_CONN_STATE_MAX];

	ITcpConnection			*m_pTcpConnection;
	CPlayer					*m_pPlayer;

	E_SERVER_CONN_STATE		m_eState;

	time_t					m_nTimeOut;
};

#endif
