#ifndef __I_CENTER_SERVER_LOGIC_H_
#define __I_CENTER_SERVER_LOGIC_H_

class IClientConnection;

class ICenterServerLogic
{
public:
	virtual bool			Initialize() = 0;
	virtual void			Run() = 0;
	virtual bool			AppClientLogin(IClientConnection *pAppConnection) = 0;
	virtual void			AppClientLogout(IClientConnection *pAppConnection) = 0;
	virtual bool			WebClientLogin(IClientConnection *pAppConnection) = 0;
	virtual void			WebClientLogout(IClientConnection *pAppConnection) = 0;
	virtual bool			DataClientLogin(IClientConnection *pAppConnection) = 0;
	virtual void			DataClientLogout(IClientConnection *pAppConnection) = 0;
};

extern ICenterServerLogic	&g_ICenterServerLogic;

#endif
