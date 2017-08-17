#ifndef __I_MYSQL_QUERY_H_
#define __I_MYSQL_QUERY_H_

#include "commondefine.h"

class IMysqlQuery
{
public:
	virtual bool		SendDBRequest(const void *pPack, const unsigned int uPackLen) = 0;
	virtual const void	*GetDBRespond(unsigned int &uPackLen) = 0;
	virtual void		Stop() = 0;
	virtual bool		IsExit() = 0;
	virtual void		Release() = 0;
};

struct SMysqlRequest
{
	uint64	uClientID;
	UINT	uClientIndex;
	WORD	wSqlLen;
	BYTE	byClientType;
	BYTE	byOpt;
};

struct SMysqlRespond
{
	uint64	uClientID;
	UINT	uClientIndex;
	UINT	uRowCount;
	UINT	uColCount;
	int		nRetCode;
	BYTE	byClientType;
	BYTE	byOpt;
};

struct SMysqlDataHead
{
	UINT	uOffset;
	UINT	uDataLen;
};

IMysqlQuery				*CreateMysqlQuery(const char *pstrSettingFile, const char *pstrSection);

#endif
