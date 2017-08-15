#include "stdafx.h"
#include "my_global.h"
#include "mysql.h"
#include "MysqlResult.h"

CMysqlResult::CMysqlResult()
{
	m_pDataBuffer		= nullptr;
	m_pResultHead		= nullptr;
	m_pDataHead			= nullptr;

	m_uBufferLen		= 0;
	m_uRowCount			= 0;
	m_uColCount			= 0;
	m_uOffset			= 0;
}

CMysqlResult::~CMysqlResult()
{
	SAFE_DELETE_ARR(m_pDataBuffer);
}

bool CMysqlResult::Initialize(const UINT uBufferLen)
{
	m_uBufferLen	= uBufferLen;
	if (0 == m_uBufferLen)
	{
		return false;
	}

	m_pDataBuffer	= new char[m_uBufferLen];
	if (nullptr == m_pDataBuffer)
	{
		return false;
	}

	m_pResultHead	= (SMysqlResultHead*)m_pDataBuffer;
	m_pDataHead		= (SMysqlDataHead*)(m_pDataBuffer+sizeof(SMysqlResultHead));

	return true;
}

void CMysqlResult::Clear()
{
	m_uRowCount	= 0;
	m_uColCount	= 0;

	m_uOffset	= 0;

	m_pResultHead->uColCount	= 0;
	m_pResultHead->uRowCount	= 0;
	m_pResultHead->nRetCode		= 0;
}

bool CMysqlResult::AddResult(const UINT uRow, const UINT uCol, const char *pstrData, const UINT uDataLen)
{
	if (uRow >= m_uRowCount || uCol >= m_uColCount || nullptr == pstrData || 0 == uDataLen)
		return false;

	if (m_uOffset + uDataLen > m_uBufferLen)
	{
		Clear();
		return false;
	}

	auto	nIndex = m_uColCount * uRow + uCol;
	m_pDataHead[nIndex].uDataLen	= uDataLen;
	m_pDataHead[nIndex].uOffset		= m_uOffset;

	memcpy(m_pDataBuffer + m_uOffset, pstrData, uDataLen);

	m_uOffset += uDataLen;

	return true;
}
