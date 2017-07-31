#ifndef __I_CENTER_SERVER_LOGIC_H_
#define __I_CENTER_SERVER_LOGIC_H_

class IClientConnection;

class ICenterServerLogic
{
public:
	virtual bool			Initialize() = 0;
	virtual void			Run() = 0;
	virtual bool			AppLogin(IClientConnection *pAppConnection) = 0;
	virtual void			AppLogout(IClientConnection *pAppConnection) = 0;
	virtual bool			WebLogin(IClientConnection *pAppConnection) = 0;
	virtual void			WebLogout(IClientConnection *pAppConnection) = 0;
};

extern ICenterServerLogic	&g_ICenterServerLogic;

#endif
