#ifndef __MYSQL_RESULT_H_
#define __MYSQL_RESULT_H_

#include "IMysqlQuery.h"

class CMysqlResult : public IMysqlResult
{
public:
	CMysqlResult();
	~CMysqlResult();

	inline uint32_t			GetRowCount()
	{
		return m_pResultHead->uRowCount;
	}

	inline uint32_t			GetColCount()
	{
		return m_pResultHead->uColCount;
	}

	bool					GetData(const uint32_t uRow, const uint32_t uCol, int64_t &nData);
	bool					GetData(const uint32_t uRow, const uint32_t uCol, uint64_t &uData);
	bool					GetData(const uint32_t uRow, const uint32_t uCol, int &nData);
	bool					GetData(const uint32_t uRow, const uint32_t uCol, uint32_t &uData);
	bool					GetData(const uint32_t uRow, const uint32_t uCol, short &sData);
	bool					GetData(const uint32_t uRow, const uint32_t uCol, uint16_t &wData);
	bool					GetData(const uint32_t uRow, const uint32_t uCol, uint8_t &byData);
	bool					GetData(const uint32_t uRow, const uint32_t uCol, double &dData);
	uint32_t				GetData(const uint32_t uRow, const uint32_t uCol, char *pstrParam, const unsigned int uSize);
	uint32_t				GetData(const uint32_t uRow, const uint32_t uCol, void *pParam, const unsigned int uSize);

	bool					ParseResult(char *pstrBuffer);
private:
	char					*GetDataString(const uint32_t uRow, const uint32_t uCol, unsigned int &uSize);
private:
	SResultHead				*m_pResultHead;
	SMysqlDataHead			*m_pDataHead;
	char					*m_pData;
};

#endif
