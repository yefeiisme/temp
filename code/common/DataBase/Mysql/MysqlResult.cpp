#include "stdafx.h"
#include "my_global.h"
#include "mysql.h"
#include "MysqlResult.h"

CMysqlResult::CMysqlResult()
{
	m_pBuffer		= nullptr;
	m_pResultHead	= nullptr;
	m_pDataHead		= nullptr;

	m_uBufferLen	= 0;

	m_uOffset		= 0;

	m_nReturnCode	= 0;
}

CMysqlResult::~CMysqlResult()
{
	SAFE_DELETE_ARR(m_pBuffer);
}

char *CMysqlResult::GetDataString(const UINT uRow, const UINT uCol, unsigned int &uSize)
{
	if (0 == m_pResultHead->uRowCount || 0 == m_pResultHead->uColCount)
		return nullptr;

	if (uRow >= m_pResultHead->uRowCount || uCol >= m_pResultHead->uColCount)
		return nullptr;

	auto	nIndex		= m_pResultHead->uColCount * uRow + uCol;
	auto	uMinSize	= min(m_pDataHead[nIndex].uDataLen, uSize);

	uSize	= m_pDataHead[nIndex].uDataLen;
	return (m_pBuffer+m_pDataHead[nIndex].uOffset);
}

bool CMysqlResult::GetData(const UINT uRow, const UINT uCol, int &nData)
{
	char	strResultData[64]	= {0};

	if (!GetData(uRow, uCol, strResultData, sizeof(strResultData)))
		return false;

	nData	= atoi(strResultData);

	return true;
}

bool CMysqlResult::GetData(const UINT uRow, const UINT uCol, unsigned int &uData)
{
	char	strResultData[64]	= {0};

	if (!GetData(uRow, uCol, strResultData, sizeof(strResultData)))
		return false;

	uData	= strtoul(strResultData, nullptr, 10);

	return true;
}

bool CMysqlResult::GetData(const UINT uRow, const UINT uCol, short &sData)
{
	char	strResultData[64]	= {0};

	if (!GetData(uRow, uCol, strResultData, sizeof(strResultData)))
		return false;

	sData	= atoi(strResultData);

	return true;
}

bool CMysqlResult::GetData(const UINT uRow, const UINT uCol, unsigned short &wData)
{
	char	strResultData[64]	= {0};

	if (!GetData(uRow, uCol, strResultData, sizeof(strResultData)))
		return false;

	wData	= strtoul(strResultData, nullptr, 10);

	return true;
}

bool CMysqlResult::GetData(const UINT uRow, const UINT uCol, unsigned char &byData)
{
	char	strResultData[64]	= {0};

	if (!GetData(uRow, uCol, strResultData, sizeof(strResultData)))
		return false;

	byData	= strtoul(strResultData, nullptr, 10);

	return true;
}

bool CMysqlResult::GetData(const UINT uRow, const UINT uCol, double &dData)
{
	char	strResultData[64]	={ 0 };

	if (!GetData(uRow, uCol, strResultData, sizeof(strResultData)))
		return false;

	dData	= atof(strResultData);

	return true;
}

UINT CMysqlResult::GetData(const UINT uRow, const UINT uCol, char *pstrParam, const unsigned int uSize)
{
	if (nullptr == pstrParam || 0 == uSize)
		return 0;

	UINT	uStringSize			= 0;
	char	*pstrResultString	= GetDataString(uRow, uCol, uStringSize);

	if (nullptr == pstrResultString)
		return 0;

	int	nCopySize	= uSize > uStringSize ? uStringSize : uSize - 1;
	memcpy(pstrParam, pstrResultString, uSize);
	pstrResultString[nCopySize]	= '\0';

	return (nCopySize - 1);
}

UINT CMysqlResult::GetData(const UINT uRow, const UINT uCol, void *pParam, const unsigned int uSize)
{
	if (nullptr == pParam || 0 == uSize)
		return 0;

	UINT	uStringSize			= 0;
	char	*pstrResultString	= GetDataString(uRow, uCol, uStringSize);

	if (nullptr == pstrResultString)
		return 0;

	int	nCopySize	= uSize > uStringSize ? uStringSize : uSize;
	memcpy(pParam, pstrResultString, uSize);

	return nCopySize;
}

bool CMysqlResult::Initialize(const UINT uBufferLen)
{
	m_uBufferLen	= uBufferLen;
	if (0 == m_uBufferLen)
	{
		return false;
	}

	m_pBuffer	= new char[m_uBufferLen];
	if (nullptr == m_pBuffer)
	{
		return false;
	}

	m_pResultHead	= (SMysqlRespond*)m_pBuffer;
	m_pDataHead		= (SMysqlDataHead*)(m_pBuffer+sizeof(SMysqlRespond));

	return true;
}

void CMysqlResult::Clear()
{
	m_uOffset		= 0;

	m_nReturnCode	= 0;

	m_pResultHead->uColCount	= 0;
	m_pResultHead->uRowCount	= 0;
	m_pResultHead->nRetCode		= 0;
}

bool CMysqlResult::AddResult(const UINT uRow, const UINT uCol, const char *pstrData, const UINT uDataLen)
{
	if (0 == m_pResultHead->uRowCount || 0 == m_pResultHead->uColCount)
		return false;

	if (uRow >= m_pResultHead->uRowCount || uCol >= m_pResultHead->uColCount || nullptr == pstrData || 0 == uDataLen)
		return false;

	if (m_uOffset + uDataLen > m_uBufferLen)
	{
		Clear();
		return false;
	}

	auto	nIndex = m_pResultHead->uColCount * uRow + uCol;
	m_pDataHead[nIndex].uDataLen	= uDataLen;
	m_pDataHead[nIndex].uOffset		= m_uOffset;

	memcpy(m_pBuffer + m_uOffset, pstrData, uDataLen);

	m_uOffset += uDataLen;

	return true;
}

bool CMysqlResult::ParseResult(const void *pPack, const UINT uPackLen)
{
	if (nullptr == pPack || 0 == uPackLen)
		return false;

	m_wCallbackDataLen	= *(WORD*)pPack;
	m_pCallbackData		= (char*)pPack + sizeof(WORD);

	return true;
}
