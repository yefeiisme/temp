#ifndef __CLIENT_CONNECTION_H_
#define __CLIENT_CONNECTION_H_

#include "INetwork.h"
#include "IClientConnection.h"
#include "IDataServer.h"

class CClientConnection : public IClientConnection
{
protected:
	enum E_CLIENT_CONN_STATE
	{
		CLIENT_CONN_STATE_IDLE,
		CLIENT_CONN_STATE_WAIT_LOGIN,
		CLIENT_CONN_STATE_RUNNING,
		CLIENT_CONN_STATE_MAX,
	};
public:
	CClientConnection();
	~CClientConnection();

	inline bool				IsIdle()
	{
		return CLIENT_CONN_STATE_IDLE == m_eState;
	}

	inline bool				IsTimeOut()
	{
		return g_nTimeNow > m_nTimeOut;
	}

	inline void				Connect(ITcpConnection *pTcpConnection)
	{
		m_pTcpConnection	= pTcpConnection;

		m_nTimeOut			= g_nTimeNow + 10;

		m_eState			= CLIENT_CONN_STATE_WAIT_LOGIN;
	}

	void					DoAction();

	const void				*GetPack(unsigned int &uPackLen);
	bool					PutPack(const void *pPack, unsigned int uPackLen);
protected:
	virtual void			ResetTimeOut();
	virtual void			Disconnect();

	void					OnIdle();
	virtual void			OnWaitLogin();
	void					OnRunning();
protected:
	typedef void (CClientConnection::*StateFuncArray)();
	static StateFuncArray	m_pfnStateFunc[CLIENT_CONN_STATE_MAX];

	ITcpConnection			*m_pTcpConnection;

	E_CLIENT_CONN_STATE		m_eState;

	int						m_nTimeOut;
};

#endif
