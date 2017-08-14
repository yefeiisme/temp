#include "stdafx.h"
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

	m_pDataBuffer		= new char[m_uBufferLen];
	if (nullptr == m_pDataBuffer)
	{
		return false;
	}

	return true;
}

void CMysqlResult::Clear()
{
	m_uRowCount	= 0;
	m_uColCount	= 0;

	m_uOffset	= 0;
}

bool CMysqlResult::AddResult(const UINT uRow, const UINT uCol, const char *pstrData, const UINT uDataLen)
{
	if (uRow >= m_uRowCount)
		return false;

	if (m_uOffset + uDataLen > m_uBufferLen)
	{
		Clear();
		return false;
	}

	return true;
}
