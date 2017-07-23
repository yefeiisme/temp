#ifndef __I_CENTER_SERVER_LOGIC_H_
#define __I_CENTER_SERVER_LOGIC_H_

class IAppConnection;

class ICenterServerLogic
{
public:
	virtual bool			Initialize() = 0;
	virtual void			Run() = 0;
	virtual bool			AppLogin(IAppConnection *pAppConnection) = 0;
	virtual void			AppLogout(IAppConnection *pAppConnection) = 0;
};

extern ICenterServerLogic	&g_ICenterServerLogic;

#endif
