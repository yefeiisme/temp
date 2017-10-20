#ifndef __SIMULATOR_LOGIC_H_
#define __SIMULATOR_LOGIC_H_

#include "INetwork.h"
#include "ISimulatorLogic.h"
#include "IRingBuffer.h"
#include "UI2LogicProtocol.h"
#if defined (WIN32) || defined (WIN64)
#include <windows.h>
#else
#endif

using namespace std;

class CAppServerConnection;
class CWebServerConnection;

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

	void						ShutDownAppServerConnection();
	void						ShutDownWebServerConnection();
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
	void						ProcessConnection();
	void						ProcessRequest();

	static void					AppClientConnected(void *pParam, ITcpConnection *pTcpConnection, const unsigned int uIndex);
	void						OnAppClientConnect(ITcpConnection *pTcpConnection, const unsigned int uIndex);

	static void					WebServerConnected(void *pParam, ITcpConnection *pTcpConnection, const unsigned int uIndex);
	void						OnWebServerConnected(ITcpConnection *pTcpConnection, const unsigned int uIndex);
private:
	void						RecvAppLogin(const void *pPack, const unsigned int uPackLen);
	void						RecvWebLogin(const void *pPack, const unsigned int uPackLen);
	void						RecvAppSlopeList(const void *pPack, const unsigned int uPackLen);
	void						RecvWebSlopeList(const void *pPack, const unsigned int uPackLen);
	void						RecvAppSensorList(const void *pPack, const unsigned int uPackLen);
	void						RecvWebSensorList(const void *pPack, const unsigned int uPackLen);
	void						RecvAppSensorHistory(const void *pPack, const unsigned int uPackLen);
	void						RecvWebSensorHistory(const void *pPack, const unsigned int uPackLen);
private:
	typedef void (CSimulatorLogic::*pfnProtocolFunc)(const void *pPack, const unsigned int uPackLen);
	pfnProtocolFunc				m_ProtocolFunc[u2l_end];

	IClientNetwork				*m_pAppClientNetwork;
	IClientNetwork				*m_pWebClientNetwork;

	IRingBuffer					*m_pRBRequest;
	IRingBuffer					*m_pRBRespond;

	CAppServerConnection		*m_pAppServerConnList;
	CWebServerConnection		*m_pWebServerConnList;

	uint64						m_uFrame;

	uint64						m_ullBeginTick;
	uint64						m_ullNextFrameTick;
	uint64						m_ullTickNow;

	bool						m_bRunning;
	bool						m_bWaitExit;
};

extern CSimulatorLogic			&g_pSimulatorLogic;

#endif
