#include "stdafx.h"
#include "my_global.h"
#include "mysql.h"
#include "MysqlResult.h"
#include "MysqlResultSet.h"
#include "MysqlQuery.h"

CMysqlResultSet::CMysqlResultSet(CMysqlQuery &pMysqlQuery) : m_pMysqlQuery(pMysqlQuery)
{
	m_pMysqlResultList	= nullptr;

	m_pResultSetHead	= nullptr;
	m_pResultData		= nullptr;

	m_byMaxResultCount	= 0;

	m_vectMysqlResult.clear();
}

CMysqlResultSet::~CMysqlResultSet()
{
	SAFE_DELETE_ARR(m_pMysqlResultList);

	m_vectMysqlResult.clear();
}

bool CMysqlResultSet::Initialize(const BYTE byMaxResultCount)
{
	m_byMaxResultCount = byMaxResultCount;
	m_pMysqlResultList = new CMysqlResult[m_byMaxResultCount];
	if (nullptr == m_pMysqlResultList)
	{
		g_pFileLog->WriteLog("[%s][%d] CMysqlResultSet::Initialize New CMysqlResult[%hhu] Failed\n", __FILE__, __LINE__, m_byMaxResultCount);
		return false;
	}

	return true;
}

void CMysqlResultSet::Clear()
{
	m_pResultSetHead	= nullptr;
	m_pResultData		= nullptr;

	m_vectMysqlResult.clear();
}

bool CMysqlResultSet::CreateResultObj(const WORD wObjCount)
{
	for (auto nIndex = 0; nIndex < wObjCount; ++nIndex)
	{
		CMysqlResult	*pNewResult = new CMysqlResult;
		if (nullptr == pNewResult)
			return false;

		if (!pNewResult)
			return false;
	}
	return true;
}

bool CMysqlResultSet::ParseResult(const void *pPack, const UINT uPackLen)
{
	if (nullptr == pPack)
		return false;

	if (uPackLen < sizeof(SResultSetHead))
		return false;

	m_pResultSetHead	= (SResultSetHead*)pPack;
	if (m_pResultSetHead->byResultCount > m_byMaxResultCount)
		return false;

	m_pResultData		= (char*)pPack + sizeof(SResultSetHead);
	m_pResultHead		= (SResultHead*)m_pResultData;

	for (auto nIndex = 0; nIndex < m_pResultSetHead->byResultCount; ++nIndex)
	{
		m_pMysqlResultList[nIndex].ParseResult(m_pResultData);

		m_pResultData += m_pResultHead->uLen;
		m_pResultHead = (SResultHead*)m_pResultData;
	}

	return true;
}
