#ifndef __I_CENTER_SERVER_H_
#define __I_CENTER_SERVER_H_

class IMysqlQuery;
class IMysqlResultSet;

class ICenterServer
{
public:
	virtual bool			Initialize(const bool bDaemon) = 0;
	virtual void			Run() = 0;
	virtual void			Stop() = 0;
	virtual void			Exit() = 0;
	virtual IMysqlQuery		*GetMysqlQuery() = 0;
	virtual IMysqlResultSet	*GetQueryResult() = 0;
};

extern ICenterServer		&g_ICenterServer;
extern int					g_nTimeNow;

#endif
