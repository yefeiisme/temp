#ifndef __APP_SERVER_CONNECTION_H_
#define __APP_SERVER_CONNECTION_H_

#include "INetwork.h"
#include "ISimulatorLogic.h"
#include "App_Server_Protocol.pb.h"

class CAppServerConnection
{
	enum E_SERVER_CONN_STATE
	{
		SERVER_CONN_STATE_IDLE,
		SERVER_CONN_STATE_WAIT_CONNECT,
		SERVER_CONN_STATE_RUNNING,
		SERVER_CONN_STATE_DISCONNECT,
		SERVER_CONN_STATE_MAX,
	};
public:
	CAppServerConnection();
	~CAppServerConnection();

	inline void				SetIndex(const unsigned int uIndex)
	{
		m_uIndex = uIndex;
	}

	inline void				Connect(ITcpConnection &pTcpConnection)
	{
		m_pTcpConnection	= &pTcpConnection;
		m_nTimeOut			= g_nSimulatorSecond + 10;
	}

	void					DoAction();

	const void				*GetPack(unsigned int &uPackLen);
	bool					PutPack(const void *pPack, unsigned int uPackLen);
	void					ResetTimeOut();

	inline void				ConnectWait()
	{
		m_eState	= SERVER_CONN_STATE_WAIT_CONNECT;
		m_nTimeOut	= g_nSimulatorSecond + 10;
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
	void					OnDisconnect();
private:
	void					ProcessNetPack(const void *pPack, const unsigned int uPackLen);

	void					RecvLoginResult(const void *pPack, const unsigned int uPackLen);
private:
	typedef void (CAppServerConnection::*StateFuncArray)();
	static StateFuncArray	m_pfnStateFunc[SERVER_CONN_STATE_MAX];

	typedef void			(CAppServerConnection::*pfnProtocolFunc)(const void *pPack, const unsigned int uPackLen);
	static pfnProtocolFunc	m_ProtocolFunc[APP_SERVER_NET_Protocol::S2APP::s2app_max];

	ITcpConnection			*m_pTcpConnection;

	unsigned int			m_uIndex;
	E_SERVER_CONN_STATE		m_eState;

	time_t					m_nTimeOut;
};

#endif
