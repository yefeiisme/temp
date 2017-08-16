#ifndef __CENTER_SERVER_LOGIC_H_
#define __CENTER_SERVER_LOGIC_H_

#include "IClientConnection.h"
#include "ICenterServerLogic.h"
#include <map>

using namespace std;

class CAppClient;
class CWebClient;
class CDataClient;
class CSensorDBConn;

class CCenterServerLogic : public ICenterServerLogic
{
public:
	CCenterServerLogic();
	~CCenterServerLogic();

	static CCenterServerLogic	&Singleton();

	bool						Initialize();
	void						Run();
	bool						AppClientLogin(IClientConnection *pClientConnection);
	void						AppClientLogout(IClientConnection *pClientConnection);
	bool						WebClientLogin(IClientConnection *pClientConnection);
	void						WebClientLogout(IClientConnection *pClientConnection);
	bool						DataClientLogin(IClientConnection *pClientConnection);
	void						DataClientLogout(IClientConnection *pClientConnection);

	inline CAppClient			*GetFreeAppClient()
	{
		if (m_listFreeAppClient.empty())
			return nullptr;

		CAppClient	*pAppClient	= *m_listFreeAppClient.begin();
		m_listFreeAppClient.pop_front();

		return pAppClient;
	}

	inline CWebClient			*GetFreeWebClient()
	{
		if (m_listFreeWebClient.empty())
			return nullptr;

		CWebClient	*pWebClient	= *m_listFreeWebClient.begin();
		m_listFreeWebClient.pop_front();

		return pWebClient;
	}

	inline CDataClient			*GetFreeDataClient()
	{
		if (m_listFreeDataClient.empty())
			return nullptr;

		CDataClient	*pDataClient = *m_listFreeDataClient.begin();
		m_listFreeDataClient.pop_front();

		return pDataClient;
	}
private:
	CSensorDBConn							*m_pSensorDBConn;

	CAppClient								*m_pAppClientList;
	CWebClient								*m_pWebClientList;
	CDataClient								*m_pDataClientList;

	list<CAppClient*>						m_listFreeAppClient;
	list<CWebClient*>						m_listFreeWebClient;
	list<CDataClient*>						m_listFreeDataClient;

	map<IClientConnection*,CAppClient*>		m_mapOnlineAppClient;
	map<IClientConnection*,CWebClient*>		m_mapOnlineWebClient;
	map<IClientConnection*,CDataClient*>	m_mapOnlineDataClient;
};

extern CCenterServerLogic					&g_pCenterServerLogic;

#endif
