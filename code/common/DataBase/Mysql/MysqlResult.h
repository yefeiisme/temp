#ifndef __MYSQL_RESULT_H_
#define __MYSQL_RESULT_H_

#include "IMysqlQuery.h"

class CMysqlResult
{
public:
	CMysqlResult();
	~CMysqlResult();

	bool					Initialize(const UINT uBufferLen);
	void					Clear();

	inline bool				SetResultSize(const UINT uRow, const UINT uCol)
	{
		if (sizeof(SMysqlResultHead)+sizeof(SMysqlDataHead)*uRow*uCol > m_uBufferLen)
			return false;

		m_uRowCount	= uRow;
		m_uColCount	= uCol;

		m_pResultHead->uRowCount	= uRow;
		m_pResultHead->uColCount	= uCol;

		return true;
	}

	bool					AddResult(const UINT uRow, const UINT uCol, const char *pstrData, const UINT uDataLen);
private:
	char					*m_pDataBuffer;

	SMysqlResultHead		*m_pResultHead;
	SMysqlDataHead			*m_pDataHead;

	UINT					m_uBufferLen;
	UINT					m_uRowCount;
	UINT					m_uColCount;

	UINT					m_uOffset;
};

#endif
