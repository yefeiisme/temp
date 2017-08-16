#ifndef __I_CENTER_SERVER_H_
#define __I_CENTER_SERVER_H_

class ICenterServer
{
public:
	virtual bool		Initialize(const bool bDaemon) = 0;
	virtual void		Run() = 0;
	virtual void		Stop() = 0;
	virtual void		Exit() = 0;
	virtual bool		SendDBRequest(const void *pPack, const unsigned int uPackLen) = 0;
	virtual const void	*GetDBRespond(unsigned int &uPackLen) = 0;
};

extern ICenterServer	&g_ICenterServer;
extern int				g_nTimeNow;

#endif
