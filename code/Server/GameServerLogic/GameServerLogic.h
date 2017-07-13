#ifndef __GAME_SERVER_LOGIC_H_
#define __GAME_SERVER_LOGIC_H_

#include "IGameServerLogic.h"
#include <map>

class CPlayer;

class CGameServerLogic : public IGameServerLogic
{
public:
	CGameServerLogic();
	~CGameServerLogic();

	static CGameServerLogic	&Singleton();

	bool					Initialize();
	void					Run();
	bool					ClientLogin(IClientConnection *pClientConnection);
	void					ClientLogout(IClientConnection *pClientConnection);

	inline CPlayer			*GetFreePlayer()
	{
		if (m_listFreePlayer.empty())
			return nullptr;

		CPlayer	*pPlayer	= *m_listFreePlayer.begin();
		m_listFreePlayer.pop_front();

		return pPlayer;
	}

	void					BroadCastAllPlayer(const void *pPack, const unsigned int uPackLen);
private:
	CPlayer					*m_pPlayerList;
	std::map<IClientConnection*,CPlayer*>	m_mapOnlinePlayer;
	std::list<CPlayer*>		m_listFreePlayer;
};

extern CGameServerLogic		&g_pGameServerLogic;

#endif
