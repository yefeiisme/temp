#ifndef __I_MYSQL_QUERY_H_
#define __I_MYSQL_QUERY_H_

#include "commondefine.h"

struct SMysqlRequest
{
	uint64	uClientID;
	UINT	uClientIndex;
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

class IQueryResult
{
	virtual bool			GetData(const UINT uRow, const UINT uCol, int &nData);
	virtual bool			GetData(const UINT uRow, const UINT uCol, unsigned int &uData);
	virtual bool			GetData(const UINT uRow, const UINT uCol, short &sData);
	virtual bool			GetData(const UINT uRow, const UINT uCol, unsigned short &wData);
	virtual bool			GetData(const UINT uRow, const UINT uCol, unsigned char &byData);
	virtual bool			GetData(const UINT uRow, const UINT uCol, char *pstrParam, const unsigned int uSize);
	virtual bool			GetData(const UINT uRow, const UINT uCol, void *pParam, const unsigned int uSize);
};

class IMysqlQuery
{
public:
	virtual void			Stop() = 0;
	virtual bool			IsExit() = 0;
	virtual void			Release() = 0;

	virtual void			PrepareProc(const char *pstrProcName) = 0;
	virtual bool			AddParam(const int nParam) = 0;
	virtual bool			AddParam(const unsigned int uParam) = 0;
	virtual bool			AddParam(const short sParam) = 0;
	virtual bool			AddParam(const unsigned short usParam) = 0;
	virtual bool			AddParam(const unsigned char byParam) = 0;
	virtual bool			AddParam(const char *pstrParam) = 0;
	virtual bool			AddParam(const void *pParam) = 0;
	virtual bool			EndPrepareProc(SMysqlRequest &tagRequest) = 0;
	virtual bool			CallProc() = 0;

	virtual void			Query(const void *pPack, const unsigned int uPackLen) = 0;

	virtual const void		*GetDBRespond(unsigned int &uPackLen) = 0;
	virtual IQueryResult	*GetQueryResult() = 0;
};

IMysqlQuery					*CreateMysqlQuery(const char *pstrSettingFile, const char *pstrSection);

#endif
