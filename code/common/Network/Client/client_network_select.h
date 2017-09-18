#ifndef __CLIENT_NETWORK_SELECT_H_
#define __CLIENT_NETWORK_SELECT_H_

#include "INetwork.h"
#include "IRingBuffer.h"

#define MAX_IPV4_ADDR_LEN	16

struct SConnectRequest
{
	unsigned int	uIndex;
	char			strAddr[MAX_IPV4_ADDR_LEN];
	unsigned short	usPort;
};

class CClientConnInfo;

class CClientNetwork : public IClientNetwork
{
private:
	pfnConnectEvent			m_pfnConnectCallBack;
	void					*m_pFunParam;

	typedef void			(CClientNetwork::*pfnStateFunc)(CClientConnInfo &pClientConn);
	static pfnStateFunc		m_pfnClientStateFunc[CLIENT_CONN_STATE_MAX];

	CClientConnInfo			*m_pTcpConnection;

	unsigned int			m_uMaxConnCount;

	unsigned int			m_uSleepTime;

	fd_set					m_ReadSet;
	fd_set					m_WriteSet;
	fd_set					m_ErrorSet;

	bool					m_bRunning;
	bool					m_bExited;
public:
	CClientNetwork();
	~CClientNetwork();

	bool					Initialize(
										const unsigned int uClientCount,
										const unsigned int uSendBuffLen,
										const unsigned int uRecvBuffLen,
										const unsigned int uTempSendBuffLen,
										const unsigned int uTempRecvBuffLen,
										pfnConnectEvent pfnConnectCallBack,
										void *lpParm,
										const unsigned int uSleepTime,
										const unsigned char byPackHeadSize
										);
	inline void				Stop()
	{
		m_bRunning	= false;
	}

	inline bool				IsExited()
	{
		return m_bExited;
	}

	void					Release();
	bool					ConnectTo(char *pstrAddr, const unsigned short usPort, const unsigned int uIndex);
	bool					ConnectToUrl(char *pstrAddr, const unsigned short usPort, const unsigned int uIndex);
	void					ShutDown(const unsigned int uIndex);
private:
	void					OnClientIdle(CClientConnInfo &pClientConn);
	void					OnClientTryConnect(CClientConnInfo &pClientConn);
	void					OnClientWaitConnect(CClientConnInfo &pClientConn);
	void					OnClientConnect(CClientConnInfo &pClientConn);
	void					OnClientWaitLogicExit(CClientConnInfo &pClientConn);

	int						SetNoBlocking(const SOCKET nSock);

	void					ThreadFunc();
	inline void				yield()
	{
#if defined (WIN32) || defined (WIN64)
		Sleep(m_uSleepTime);
#else
		struct timeval sleeptime;
		sleeptime.tv_sec	= 0;
		sleeptime.tv_usec	= m_uSleepTime * 1000;
		select(0, 0, 0, 0, &sleeptime);
#endif
	}
};

#endif
