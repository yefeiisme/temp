#ifndef __GAME_SERVER_H_
#define __GAME_SERVER_H_

#include "INetwork.h"
#include "IGameServer.h"

class CClientConnection;

class CGameServer : public IGameServer
{
public:
	CGameServer();
	~CGameServer();

	static CGameServer			&Singleton();

	bool						Initialize();
	void						Run();
	inline void					Stop()
	{
		m_bRunning	= false;
	}
	inline void					Exit()
	{
		m_bRunning	= false;	
		m_bWaitExit	= false;	
	}
private:
	void						ProcessClient();

	static void					ClientConnected(void *pParam, ITcpConnection *pTcpConnection, const unsigned int uIndex);
	void						OnClientConnect(ITcpConnection *pTcpConnection, const unsigned int uIndex);
private:
	IServerNetwork				*m_pClientNetwork;

	CClientConnection			*m_pClientList;

	uint64						m_uFrame;

	uint64						m_ullBeginTick;
	uint64						m_ullNextFrameTick;
	uint64						m_ullTickNow;

	UINT						m_uClientCount;

	bool						m_bRunning;
	bool						m_bWaitExit;
};

extern CGameServer				&g_pGameServer;

#endif