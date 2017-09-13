#ifndef __DATA_SERVER_LOGIC_H_
#define __DATA_SERVER_LOGIC_H_

#include "IDataConnection.h"
#include "IDataLogic.h"
#include <map>

using namespace std;

class CDataClient;
class CSensorDBConn;

class CDataLogic : public IDataLogic
{
public:
	CDataLogic();
	~CDataLogic();

	static CDataLogic						&Singleton();

	bool									Initialize();
	void									Run();

	bool									DataClientLogin(IClientConnection *pClientConnection);
	void									DataClientLogout(IClientConnection *pClientConnection);

	inline CDataClient						*GetFreeDataClient()
	{
		if (m_listFreeDataClient.empty())
			return nullptr;

		CDataClient	*pDataClient = *m_listFreeDataClient.begin();
		m_listFreeDataClient.pop_front();

		return pDataClient;
	}

	CDataClient								*GetDataClient(const UINT uClientIndex, const uint64 uClientID);
private:
	CSensorDBConn							*m_pSensorDBConn;

	CDataClient								*m_pDataClientList;

	uint64									m_uDataID;

	list<CDataClient*>						m_listFreeDataClient;

	map<IClientConnection*,CDataClient*>	m_mapOnlineDataClient;
};

extern CDataLogic							&g_pDataLogic;

#endif
