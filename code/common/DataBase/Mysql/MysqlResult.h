#ifndef __MYSQL_RESULT_H_
#define __MYSQL_RESULT_H_

#include "IMysqlQuery.h"

class CMysqlResult : public IMysqlResult
{
public:
	CMysqlResult();
	~CMysqlResult();

	inline UINT				GetRowCount()
	{
		return m_pResultHead->uRowCount;
	}

	inline UINT				GetColCount()
	{
		return m_pResultHead->uColCount;
	}

	bool					GetData(const UINT uRow, const UINT uCol, int &nData);
	bool					GetData(const UINT uRow, const UINT uCol, unsigned int &uData);
	bool					GetData(const UINT uRow, const UINT uCol, short &sData);
	bool					GetData(const UINT uRow, const UINT uCol, unsigned short &wData);
	bool					GetData(const UINT uRow, const UINT uCol, unsigned char &byData);
	bool					GetData(const UINT uRow, const UINT uCol, double &dData);
	UINT					GetData(const UINT uRow, const UINT uCol, char *pstrParam, const unsigned int uSize);
	UINT					GetData(const UINT uRow, const UINT uCol, void *pParam, const unsigned int uSize);

	bool					ParseResult(char *pstrBuffer);
private:
	char					*GetDataString(const UINT uRow, const UINT uCol, unsigned int &uSize);
private:
	SResultHead				*m_pResultHead;
	SMysqlDataHead			*m_pDataHead;
	char					*m_pData;
};

#endif
