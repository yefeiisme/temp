#ifndef __CENTER_SERVER_H_
#define __CENTER_SERVER_H_

#include "INetwork.h"
#include "ICenterServer.h"

class CAppConnection;

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
	IServerNetwork				*m_pAppNetwork;

	CAppConnection				*m_pAppConnList;

	uint64						m_uFrame;

	uint64						m_ullBeginTick;
	uint64						m_ullNextFrameTick;
	uint64						m_ullTickNow;

	UINT						m_uAppCount;

	bool						m_bRunning;
	bool						m_bWaitExit;
};

extern CCenterServer			&g_pCenterServer;

#endif
