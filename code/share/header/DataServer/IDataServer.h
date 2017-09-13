#ifndef __I_DATA_SERVER_H_
#define __I_DATA_SERVER_H_

class IMysqlQuery;
class IMysqlResultSet;

class IDataServer
{
public:
	virtual bool			Initialize(const bool bDaemon) = 0;
	virtual void			Run() = 0;
	virtual void			Stop() = 0;
	virtual void			Exit() = 0;
	virtual IMysqlQuery		*GetMysqlQuery() = 0;
	virtual IMysqlResultSet	*GetQueryResult() = 0;
};

extern IDataServer			&g_IDataServer;
extern int					g_nTimeNow;

#endif
