#ifndef __I_MYSQL_QUERY_H_
#define __I_MYSQL_QUERY_H_

#include "commondefine.h"

class IMysqlResult
{
public:
	virtual UINT			GetRowCount() = 0;
	virtual UINT			GetColCount() = 0;
	virtual bool			GetData(const UINT uRow, const UINT uCol, int &nData) = 0;
	virtual bool			GetData(const UINT uRow, const UINT uCol, unsigned int &uData) = 0;
	virtual bool			GetData(const UINT uRow, const UINT uCol, short &sData) = 0;
	virtual bool			GetData(const UINT uRow, const UINT uCol, unsigned short &wData) = 0;
	virtual bool			GetData(const UINT uRow, const UINT uCol, unsigned char &byData) = 0;
	virtual bool			GetData(const UINT uRow, const UINT uCol, double &dData) = 0;
	virtual UINT			GetData(const UINT uRow, const UINT uCol, char *pstrParam, const unsigned int uSize) = 0;
	virtual UINT			GetData(const UINT uRow, const UINT uCol, void *pParam, const unsigned int uSize) = 0;
};

class IMysqlResultSet
{
public:
	virtual char			*GetCallbackData() = 0;
	virtual WORD			GetCallbackDataLen() = 0;
	virtual BYTE			GetResultCount() = 0;
	virtual IMysqlResult	*GetMysqlResult(const BYTE byIndex) = 0;
};

class IMysqlQuery
{
public:
	virtual void			Stop() = 0;
	virtual bool			IsExit() = 0;
	virtual void			Release() = 0;

	virtual bool			PrepareProc(const char *pstrProcName) = 0;
	virtual bool			AddParam(const int nParam) = 0;
	virtual bool			AddParam(const unsigned int uParam) = 0;
	virtual bool			AddParam(const short sParam) = 0;
	virtual bool			AddParam(const unsigned short usParam) = 0;
	virtual bool			AddParam(const unsigned char byParam) = 0;
	virtual bool			AddParam(const float fParam) = 0;
	virtual bool			AddParam(const double dParam) = 0;
	virtual bool			AddParam(const char *pstrParam) = 0;
	virtual bool			AddParam(const void *pParam) = 0;
	virtual bool			EndPrepareProc(void *pCallbackData, const WORD wDataLen) = 0;
	virtual bool			CallProc() = 0;

	virtual const void		*GetDBRespond(unsigned int &uPackLen) = 0;
	virtual IMysqlResultSet	*GetMysqlResultSet() = 0;
};

IMysqlQuery					*CreateMysqlQuery(const char *pstrSettingFile, const char *pstrSection);

#endif
