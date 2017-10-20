#ifndef __WEB_SERVER_CONNECTION_H_
#define __WEB_SERVER_CONNECTION_H_

#include "INetwork.h"
#include "ISimulatorLogic.h"
#include "Web_Server_Protocol.pb.h"

class CWebServerConnection
{
	enum WEB_SERVER_CONN_STATE
	{
		WEB_SERVER_CONN_STATE_IDLE,
		WEB_SERVER_CONN_STATE_WAIT_CONNECT,
		WEB_SERVER_CONN_STATE_RUNNING,
		WEB_SERVER_CONN_STATE_DISCONNECT,
		WEB_SERVER_CONN_STATE_MAX,
	};
public:
	CWebServerConnection();
	~CWebServerConnection();

	inline void				Connect(ITcpConnection &pTcpConnection)
	{
		m_pTcpConnection	= &pTcpConnection;
		m_nTimeOut			= g_nSimulatorSecond + 10;
	}

	void					DoAction();

	const void				*GetPack(unsigned int &uPackLen);
	bool					PutPack(const void *pPack, unsigned int uPackLen);
	void					ResetTimeOut();

	inline bool				IsIdle()
	{
		return WEB_SERVER_CONN_STATE_IDLE == m_eState;
	}

	inline bool				IsConnectWait()
	{
		return WEB_SERVER_CONN_STATE_WAIT_CONNECT == m_eState;
	}

	inline bool				IsTimeOut()
	{
		return g_nSimulatorSecond > m_nTimeOut;
	}
private:
	inline void				ChangeState(const WEB_SERVER_CONN_STATE eState, const int nTimeOut = 10)
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
	void					RecvSlopeList(const void *pPack, const unsigned int uPackLen);
	void					RecvSensorList(const void *pPack, const unsigned int uPackLen);
	void					RecvSensorHistory(const void *pPack, const unsigned int uPackLen);
	void					RecvAllList(const void *pPack, const unsigned int uPackLen);
private:
	void					ParseFromPackage(google::protobuf::Message &tagMsg, const void *pPack, const unsigned int uPackLen);
private:
	typedef void (CWebServerConnection::*StateFuncArray)();
	static StateFuncArray	m_pfnStateFunc[WEB_SERVER_CONN_STATE_MAX];

	typedef void			(CWebServerConnection::*pfnProtocolFunc)(const void *pPack, const unsigned int uPackLen);
	static pfnProtocolFunc	m_ProtocolFunc[WEB_SERVER_NET_Protocol::S2WEB::s2web_max];

	ITcpConnection			*m_pTcpConnection;

	unsigned int			m_uIndex;
	WEB_SERVER_CONN_STATE	m_eState;

	time_t					m_nTimeOut;
};

#endif
