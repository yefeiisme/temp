#ifndef __CENTER_SERVER_LOGIC_H_
#define __CENTER_SERVER_LOGIC_H_

#include "IClientConnection.h"
#include "ICenterServerLogic.h"
#include <map>

using namespace std;

class CAppUser;
class CWebUser;

class CCenterServerLogic : public ICenterServerLogic
{
public:
	CCenterServerLogic();
	~CCenterServerLogic();

	static CCenterServerLogic	&Singleton();

	bool						Initialize();
	void						Run();
	bool						AppLogin(IClientConnection *pClientConnection);
	void						AppLogout(IClientConnection *pClientConnection);
	bool						WebLogin(IClientConnection *pClientConnection);
	void						WebLogout(IClientConnection *pClientConnection);

	inline CAppUser				*GetFreeAppUser()
	{
		if (m_listFreeAppUser.empty())
			return nullptr;

		CAppUser	*pAppUser	= *m_listFreeAppUser.begin();
		m_listFreeAppUser.pop_front();

		return pAppUser;
	}

	inline CWebUser				*GetFreeWebUser()
	{
		if (m_listFreeWebUser.empty())
			return nullptr;

		CWebUser	*pWebUser	= *m_listFreeWebUser.begin();
		m_listFreeWebUser.pop_front();

		return pWebUser;
	}
private:
	CAppUser					*m_pAppUserList;
	CWebUser					*m_pWebUserList;

	list<CAppUser*>				m_listFreeAppUser;
	list<CWebUser*>				m_listFreeWebUser;

	map<IClientConnection*,CAppUser*>	m_mapOnlineAppUser;
	map<IClientConnection*,CWebUser*>	m_mapOnlineWebUser;
};

extern CCenterServerLogic		&g_pCenterServerLogic;

#endif
