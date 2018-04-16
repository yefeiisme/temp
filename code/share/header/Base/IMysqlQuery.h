#ifndef __I_MYSQL_QUERY_H_
#define __I_MYSQL_QUERY_H_

#include <stdint.h>

class IMysqlResult
{
public:
	virtual uint32_t		GetRowCount() = 0;
	virtual uint32_t		GetColCount() = 0;
	virtual bool			GetData(const uint32_t uRow, const uint32_t uCol, int64_t &nData) = 0;
	virtual bool			GetData(const uint32_t uRow, const uint32_t uCol, uint64_t &uData) = 0;
	virtual bool			GetData(const uint32_t uRow, const uint32_t uCol, int &nData) = 0;
	virtual bool			GetData(const uint32_t uRow, const uint32_t uCol, uint32_t &uData) = 0;
	virtual bool			GetData(const uint32_t uRow, const uint32_t uCol, short &sData) = 0;
	virtual bool			GetData(const uint32_t uRow, const uint32_t uCol, uint16_t &wData) = 0;
	virtual bool			GetData(const uint32_t uRow, const uint32_t uCol, uint8_t &byData) = 0;
	virtual bool			GetData(const uint32_t uRow, const uint32_t uCol, double &dData) = 0;
	virtual uint32_t		GetData(const uint32_t uRow, const uint32_t uCol, char *pstrParam, const unsigned int uSize) = 0;
	virtual uint32_t		GetData(const uint32_t uRow, const uint32_t uCol, void *pParam, const unsigned int uSize) = 0;
};

class IMysqlResultSet
{
public:
	virtual char			*GetCallbackData() = 0;
	virtual uint16_t		GetCallbackDataLen() = 0;
	virtual uint8_t			GetResultCount() = 0;
	virtual IMysqlResult	*GetMysqlResult(const uint8_t byIndex) = 0;
};

class IMysqlQuery
{
public:
	virtual void			Stop() = 0;
	virtual bool			IsExit() = 0;
	virtual void			Release() = 0;

	virtual bool			BeginBatchInsert(const char *pstrTableName, void *pCallbackData, const uint16_t wDataLen) = 0;
	virtual bool			AddColumn(const char *pstrColName) = 0;
	virtual bool			EndColumn() = 0;
	virtual bool			BeginAddParam() = 0;
	virtual bool			EndAddParam() = 0;
	virtual bool			BatchInsert() = 0;

	virtual bool			PrepareProc(const char *pstrProcName) = 0;
	virtual bool			AddParam(const int64_t nParam) = 0;
	virtual bool			AddParam(const uint64_t nParam) = 0;
	virtual bool			AddParam(const int nParam) = 0;
	virtual bool			AddParam(const uint32_t uParam) = 0;
	virtual bool			AddParam(const short sParam) = 0;
	virtual bool			AddParam(const uint16_t usParam) = 0;
	virtual bool			AddParam(const uint8_t byParam) = 0;
	virtual bool			AddParam(const float fParam) = 0;
	virtual bool			AddParam(const double dParam) = 0;
	virtual bool			AddParam(const char *pstrParam) = 0;
	virtual bool			AddParam(const void *pParam, const unsigned int uParamLen) = 0;
	virtual bool			EndPrepareProc(void *pCallbackData, const uint16_t wDataLen) = 0;
	virtual bool			CallProc() = 0;

	virtual const void		*GetDBRespond(unsigned int &uPackLen) = 0;
	virtual IMysqlResultSet	*GetMysqlResultSet() = 0;
};

IMysqlQuery					*CreateMysqlQuery(const char *pstrSettingFile, const char *pstrSection);

#endif
