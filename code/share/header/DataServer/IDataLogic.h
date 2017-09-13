#ifndef __I_DATA_SERVER_LOGIC_H_
#define __I_DATA_SERVER_LOGIC_H_

class IClientConnection;

class IDataLogic
{
public:
	virtual bool			Initialize() = 0;
	virtual void			Run() = 0;
	virtual bool			DataClientLogin(IClientConnection *pAppConnection) = 0;
	virtual void			DataClientLogout(IClientConnection *pAppConnection) = 0;
};

extern IDataLogic			&g_IDataLogic;

#endif
