#ifndef __I_GAME_SERVER_LOGIC_H_
#define __I_GAME_SERVER_LOGIC_H_

class IClientConnection;

class IGameServerLogic
{
public:
	virtual bool		Initialize() = 0;
	virtual void		Run() = 0;
	virtual bool		ClientLogin(IClientConnection *pClientConnection) = 0;
	virtual void		ClientLogout(IClientConnection *pClientConnection) = 0;
};

extern IGameServerLogic	&g_IGameServerLogic;

#endif
