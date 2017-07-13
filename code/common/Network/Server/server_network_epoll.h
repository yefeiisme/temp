#ifndef __SERVER_NETWORK_EPOLL_H_
#define __SERVER_NETWORK_EPOLL_H_

#include "../NetworkHead.h"

class CServerNetwork : public IServerNetwork
{
private:
	pfnConnectEvent			m_pfnConnectCallBack;
	void					*m_pFunParam;

	typedef void			(CServerNetwork::*pfnStateFunc)(CServerConnInfo &pServerConnInfo);
	static pfnStateFunc		m_pfnConnStateFunc[SERVER_CONN_STATE_MAX];

	CServerConnInfo			*m_pListenLink;

	CServerConnInfo			*m_pTcpConnection;		// ���е���������
	CServerConnInfo			**m_pFreeConn;			// ��ǰ�������״̬��CNetLink��������

	unsigned int			m_uMaxConnCount;
	unsigned int			m_uFreeConnIndex;		// m_pFreeLink������������list��iterator�÷�

	unsigned int			m_uSleepTime;

	int						m_nepfd;

	bool					m_bRunning;
	bool					m_bExited;
private:
	inline CServerConnInfo	*GetNewConnection()
	{
		return m_uFreeConnIndex >= m_uMaxConnCount ? NULL : m_pFreeConn[m_uFreeConnIndex++];
	}

	inline void				AddAvailableConnection(CServerConnInfo *pConnection)
	{
		if (m_uFreeConnIndex)
		{
			m_pFreeConn[--m_uFreeConnIndex]	= pConnection;
		}
	}

	void					OnConnIdle(CServerConnInfo &pClientConn);
	void					OnConnConnect(CServerConnInfo &pClientConn);
	void					OnConnWaitLogicExit(CServerConnInfo &pClientConn);

	int						SetNoBlocking(CServerConnInfo *pTcpConnection);
	void					AcceptClient(const SOCKET nNewSocket);

	void					DisconnectConnection(CServerConnInfo *pTcpConnection);
	void					RemoveConnection(CServerConnInfo *pTcpConnection);

	void					NetworkAction();

	void					ReadAction();

	void					ThreadFunc();
	inline void				yield()
	{
		struct timeval sleeptime;
		sleeptime.tv_sec	= 0;
		sleeptime.tv_usec	= m_uSleepTime * 1000;
		select(0, 0, 0, 0, &sleeptime);
	}
public:
	CServerNetwork();
	~CServerNetwork();

	bool					Initialize(
										const unsigned short usPort,
										void *lpParam,
										pfnConnectEvent pfnConnectCallBack,
										const unsigned int uConnectionNum,
										const unsigned int uSendBufferLen,
										const unsigned int uRecvBufferLen,
										const unsigned int uTempSendBufferLen,
										const unsigned int uTempRecvBufferLen,
										const unsigned int uSleepTime
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
};

#endif
