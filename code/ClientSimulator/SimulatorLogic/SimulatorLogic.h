#ifndef __SIMULATOR_LOGIC_H_
#define __SIMULATOR_LOGIC_H_

#include "network_interface.h"
#include "ISimulatorLogic.h"
#include "ring_buffer_interface.h"
#if defined (WIN32) || defined (WIN64)
#include <windows.h>
#else
#endif
using namespace std;

class CServerConnection;

class CSimulatorLogic : public ISimulatorLogic
{
public:
	CSimulatorLogic();
	~CSimulatorLogic();

	static CSimulatorLogic		&Singleton();

	bool						Initialize();
	bool						SendRequest(const void *pPack, const unsigned int uPackLen);
	const void					*GetRespond(unsigned int &uPackLen);

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
	inline void					yield(const unsigned int uSleepTime)
	{
#if defined (WIN32) || defined (WIN64)
		Sleep(uSleepTime);
#else
		struct timeval sleeptime;
		sleeptime.tv_sec	= 0;
		sleeptime.tv_usec	= uSleepTime * 1000;
		select(0, 0, 0, 0, &sleeptime);
#endif
	}

	void						ThreadFunc();
	void						ProcessClient();
	void						ProcessRequest();

	static void					ClientConnected(void *pParam, ITcpConnection *pTcpConnection, const void *pTarget);
	void						OnClientConnect(ITcpConnection *pTcpConnection, const void *pTarget);
private:
	IClientNetwork				*m_pClientNetwork;
	IRingBuffer					*m_pRBRequest;
	IRingBuffer					*m_pRBRespond;

	CServerConnection			*m_pServerConnList;

	uint64						m_uFrame;

	uint64						m_ullBeginTick;
	uint64						m_ullNextFrameTick;
	uint64						m_ullTickNow;

	bool						m_bRunning;
	bool						m_bWaitExit;

	list<CServerConnection*>	m_listFreeConn;
};

extern CSimulatorLogic			&g_pSimulatorLogic;

#endif
