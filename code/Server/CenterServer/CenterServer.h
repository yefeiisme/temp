#ifndef __CENTER_SERVER_H_
#define __CENTER_SERVER_H_

#include "INetwork.h"
#include "ICenterServer.h"

class CAppConnection;
class CWebConnection;
class CDataConnection;

class CCenterServer : public ICenterServer
{
public:
	CCenterServer();
	~CCenterServer();

	static CCenterServer		&Singleton();

	bool						Initialize(const bool bDaemon);
	void						Run();
	inline void					Stop()
	{
		m_bRunning = false;
	}
	inline void					Exit()
	{
		m_bRunning = false;
		m_bWaitExit = false;
	}
private:
	void						ProcessAppConn();
	void						ProcessWebConn();
	void						ProcessDataConn();

	static void					AppConnConnected(void *pParam, ITcpConnection *pTcpConnection, const unsigned int uIndex);
	void						OnAppConnConnect(ITcpConnection *pTcpConnection, const unsigned int uIndex);

	static void					WebConnConnected(void *pParam, ITcpConnection *pTcpConnection, const unsigned int uIndex);
	void						OnWebConnConnect(ITcpConnection *pTcpConnection, const unsigned int uIndex);

	static void					DataConnConnected(void *pParam, ITcpConnection *pTcpConnection, const unsigned int uIndex);
	void						OnDataConnConnect(ITcpConnection *pTcpConnection, const unsigned int uIndex);
private:
	IServerNetwork				*m_pAppNetwork;
	IServerNetwork				*m_pWebNetwork;
	IServerNetwork				*m_pDataNetwork;

	CAppConnection				*m_pAppConnList;
	CWebConnection				*m_pWebConnList;
	CDataConnection				*m_pDataConnList;

	uint64						m_uFrame;

	uint64						m_ullBeginTick;
	uint64						m_ullNextFrameTick;
	uint64						m_ullTickNow;

	UINT						m_uAppConnCount;
	UINT						m_uWebConnCount;
	UINT						m_uDataConnCount;

	bool						m_bRunning;
	bool						m_bWaitExit;
};

extern CCenterServer			&g_pCenterServer;

#endif
