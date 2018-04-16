#include "../stdafx.h"
#include "my_global.h"
#include "mysql.h"
#include "MysqlResult.h"

CMysqlResult::CMysqlResult()
{
	m_pResultHead	= nullptr;
	m_pDataHead		= nullptr;
	m_pData			= nullptr;
}

CMysqlResult::~CMysqlResult()
{
}

bool CMysqlResult::GetData(const uint32_t uRow, const uint32_t uCol, int64_t &nData)
{
	char	strResultData[64] = { 0 };

	if (!GetData(uRow, uCol, strResultData, sizeof(strResultData)))
		return false;

	nData = strtoll(strResultData, NULL, 10);

	return true;
}

bool CMysqlResult::GetData(const uint32_t uRow, const uint32_t uCol, uint64_t &uData)
{
	char	strResultData[64] = { 0 };

	if (!GetData(uRow, uCol, strResultData, sizeof(strResultData)))
		return false;

	uData = strtoull(strResultData, NULL, 10);

	return true;
}

bool CMysqlResult::GetData(const uint32_t uRow, const uint32_t uCol, int &nData)
{
	char	strResultData[64]	= {0};

	if (!GetData(uRow, uCol, strResultData, sizeof(strResultData)))
		return false;

	nData = strtol(strResultData, NULL, 10);

	return true;
}

bool CMysqlResult::GetData(const uint32_t uRow, const uint32_t uCol, unsigned int &uData)
{
	char	strResultData[64]	= {0};

	if (!GetData(uRow, uCol, strResultData, sizeof(strResultData)))
		return false;

	uData	= strtoul(strResultData, nullptr, 10);

	return true;
}

bool CMysqlResult::GetData(const uint32_t uRow, const uint32_t uCol, short &sData)
{
	char	strResultData[64]	= {0};

	if (!GetData(uRow, uCol, strResultData, sizeof(strResultData)))
		return false;

	sData = strtol(strResultData, nullptr, 10);

	return true;
}

bool CMysqlResult::GetData(const uint32_t uRow, const uint32_t uCol, unsigned short &wData)
{
	char	strResultData[64]	= {0};

	if (!GetData(uRow, uCol, strResultData, sizeof(strResultData)))
		return false;

	wData	= strtoul(strResultData, nullptr, 10);

	return true;
}

bool CMysqlResult::GetData(const uint32_t uRow, const uint32_t uCol, unsigned char &byData)
{
	char	strResultData[64]	= {0};

	if (!GetData(uRow, uCol, strResultData, sizeof(strResultData)))
		return false;

	byData	= strtoul(strResultData, nullptr, 10);

	return true;
}

bool CMysqlResult::GetData(const uint32_t uRow, const uint32_t uCol, double &dData)
{
	char	strResultData[64]	={ 0 };

	if (!GetData(uRow, uCol, strResultData, sizeof(strResultData)))
		return false;

	dData	= atof(strResultData);

	return true;
}

uint32_t CMysqlResult::GetData(const uint32_t uRow, const uint32_t uCol, char *pstrParam, const unsigned int uSize)
{
	if (nullptr == pstrParam || 0 == uSize)
		return 0;

	uint32_t	uStringSize			= 0;
	char		*pstrResultString	= GetDataString(uRow, uCol, uStringSize);

	if (nullptr == pstrResultString)
		return 0;

	int	nCopySize	= uSize > uStringSize ? uStringSize : uSize - 1;
	memcpy(pstrParam, pstrResultString, nCopySize);
	pstrParam[nCopySize] = '\0';

	return nCopySize;
}

uint32_t CMysqlResult::GetData(const uint32_t uRow, const uint32_t uCol, void *pParam, const unsigned int uSize)
{
	if (nullptr == pParam || 0 == uSize)
		return 0;

	uint32_t	uStringSize			= 0;
	char		*pstrResultString	= GetDataString(uRow, uCol, uStringSize);

	if (nullptr == pstrResultString)
		return 0;

	int	nCopySize	= uSize > uStringSize ? uStringSize : uSize;
	memcpy(pParam, pstrResultString, nCopySize);

	return nCopySize;
}

bool CMysqlResult::ParseResult(char *pstrBuffer)
{
	m_pResultHead	= (SResultHead*)pstrBuffer;
	if (0 == m_pResultHead->uRowCount || 0 == m_pResultHead->uColCount || 0 == m_pResultHead->uLen)
		return false;

	m_pDataHead		= (SMysqlDataHead*)(pstrBuffer + sizeof(SResultHead));
	m_pData			= pstrBuffer + sizeof(SResultHead) + sizeof(SMysqlDataHead)*m_pResultHead->uRowCount*m_pResultHead->uColCount;

	for (auto uRowIndex = 0; uRowIndex < m_pResultHead->uRowCount; ++uRowIndex)
	{
		for (auto uColIndex = 0; uColIndex < m_pResultHead->uColCount; ++uColIndex)
		{
		}
	}

	return true;
}

char *CMysqlResult::GetDataString(const uint32_t uRow, const uint32_t uCol, unsigned int &uSize)
{
	if (0 == m_pResultHead->uRowCount || 0 == m_pResultHead->uColCount)
		return nullptr;

	if (uRow >= m_pResultHead->uRowCount || uCol >= m_pResultHead->uColCount)
		return nullptr;

	auto	nIndex = m_pResultHead->uColCount * uRow + uCol;

	uSize = m_pDataHead[nIndex].uDataLen;

	return (m_pData + m_pDataHead[nIndex].uOffset);
}
