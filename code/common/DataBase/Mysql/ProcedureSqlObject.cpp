#include "stdafx.h"
#include "my_global.h"
#include "mysql.h"
#include "ProcedureSqlObject.h"
#include "MysqlQuery.h"

CProcObj::CProcObj(CMysqlQuery &pMysqlQuery) : m_pQuery(pMysqlQuery)
{
	m_pDBHandle		= nullptr;

	m_strBuffer		= nullptr;
	m_pRequest		= nullptr;
	m_strSQL		= nullptr;

	m_uBufferLen	= 0;
	m_uMaxSQLLen	= 0;
	m_uSQLLen	= 0;

	m_bAddParam		= false;
}

CProcObj::~CProcObj()
{
	SAFE_DELETE(m_strBuffer);
}

bool CProcObj::Initialize(const UINT uQueryBufferLen, MYSQL &pDBHandle)
{
	m_uMaxSQLLen	= uQueryBufferLen;
	if (0 == m_uMaxSQLLen)
	{
		g_pFileLog->WriteLog("[%s][%d] SQL Buffer Len[%u] Error\n", __FUNCTION__, __LINE__, m_uMaxSQLLen);
		return false;
	}

	m_uBufferLen	= m_uMaxSQLLen + sizeof(SMysqlRequest);
	m_strBuffer	= new char[m_uBufferLen];
	if (nullptr == m_strBuffer)
	{
		g_pFileLog->WriteLog("[%s][%d] new m_strBuffer[%u] Failed\n", __FUNCTION__, __LINE__, m_uBufferLen);
		return false;
	}

	m_pRequest	= (SMysqlRequest*)m_strBuffer;
	m_strSQL	= m_strBuffer + sizeof(SMysqlRequest);

	return true;
}

void CProcObj::PrepareProc(const char *pstrProcName)
{
	Clear();

	m_uSQLLen += snprintf(m_strSQL, m_uMaxSQLLen - m_uSQLLen, "CALL %s (", pstrProcName);
}

bool CProcObj::AddParam(const int nParam)
{
	if (m_uMaxSQLLen == m_uSQLLen)
		return false;

	if (m_bAddParam)
	{
		m_uSQLLen += snprintf(m_strSQL + m_uSQLLen, m_uMaxSQLLen - m_uSQLLen, ",%d", nParam);
	}
	else
	{
		m_uSQLLen += snprintf(m_strSQL + m_uSQLLen, m_uMaxSQLLen - m_uSQLLen, "%d", nParam);
	}

	m_bAddParam = true;

	return true;
}

bool CProcObj::AddParam(const unsigned int uParam)
{
	if (m_uMaxSQLLen == m_uSQLLen)
		return false;

	if (m_bAddParam)
	{
		m_uSQLLen += snprintf(m_strSQL + m_uSQLLen, m_uMaxSQLLen - m_uSQLLen, ",%u", uParam);
	}
	else
	{
		m_uSQLLen += snprintf(m_strSQL + m_uSQLLen, m_uMaxSQLLen - m_uSQLLen, "%u", uParam);
	}

	m_bAddParam = true;

	return true;
}

bool CProcObj::AddParam(const short sParam)
{
	if (m_uMaxSQLLen == m_uSQLLen)
		return false;

	if (m_bAddParam)
	{
		m_uSQLLen += snprintf(m_strSQL + m_uSQLLen, m_uMaxSQLLen - m_uSQLLen, ",%hd", sParam);
	}
	else
	{
		m_uSQLLen += snprintf(m_strSQL + m_uSQLLen, m_uMaxSQLLen - m_uSQLLen, "%hd", sParam);
	}

	m_bAddParam = true;

	return true;
}

bool CProcObj::AddParam(const unsigned short usParam)
{
	if (m_uMaxSQLLen == m_uSQLLen)
		return false;

	if (m_bAddParam)
	{
		m_uSQLLen += snprintf(m_strSQL + m_uSQLLen, m_uMaxSQLLen - m_uSQLLen, ",%hu", usParam);
	}
	else
	{
		m_uSQLLen += snprintf(m_strSQL + m_uSQLLen, m_uMaxSQLLen - m_uSQLLen, "%hu", usParam);
	}

	m_bAddParam = true;

	return true;
}

bool CProcObj::AddParam(const unsigned char byParam)
{
	if (m_uMaxSQLLen == m_uSQLLen)
		return false;

	if (m_bAddParam)
	{
		m_uSQLLen += snprintf(m_strSQL + m_uSQLLen, m_uMaxSQLLen - m_uSQLLen, ",%hhu", byParam);
	}
	else
	{
		m_uSQLLen += snprintf(m_strSQL + m_uSQLLen, m_uMaxSQLLen - m_uSQLLen, "%hhu", byParam);
	}

	m_bAddParam = true;

	return true;
}

bool CProcObj::AddParam(const char *pstrParam)
{
	size_t	nParamLen	= strlen(pstrParam);

	if (m_uMaxSQLLen - m_uSQLLen < nParamLen * 2 + 3)
		return false;

	if (m_bAddParam)
	{
		m_uSQLLen += snprintf(m_strSQL + m_uSQLLen, m_uMaxSQLLen - m_uSQLLen, ",'");
	}
	else
	{
		m_uSQLLen += snprintf(m_strSQL + m_uSQLLen, m_uMaxSQLLen - m_uSQLLen, "'");
	}

	m_uSQLLen += mysql_real_escape_string(m_pDBHandle, m_strSQL + m_uSQLLen, pstrParam, nParamLen);

	m_uSQLLen += snprintf(m_strSQL + m_uSQLLen, m_uMaxSQLLen - m_uSQLLen, "'");

	m_bAddParam = true;

	return true;
}

bool CProcObj::AddParam(const void *pParam)
{
	return true;
}

bool CProcObj::EndPrepareProc(SMysqlRequest &tagRequest)
{
	if (m_uSQLLen >= m_uMaxSQLLen)
		return false;

	m_uSQLLen += snprintf(m_strSQL + m_uSQLLen, m_uMaxSQLLen - m_uSQLLen, ");SELECT @result");

	memcpy(m_pRequest, &tagRequest, sizeof(SMysqlRequest));

	return true;
}

bool CProcObj::Query(const void *pPack, const unsigned int uPackLen)
{
	if (uPackLen <= sizeof(SMysqlRequest))
	{
		return false;
	}

	UINT	uSqlLen	= uPackLen - sizeof(SMysqlRequest);

	if (0 == uSqlLen || uSqlLen >= m_uMaxSQLLen)
	{
		return false;
	}

	m_uSQLLen		= uPackLen;
	char	*pstrSql	= (char*)pPack + sizeof(SMysqlRequest);
	memcpy(m_pRequest, pPack, sizeof(SMysqlRequest));
	strncpy(m_strSQL, pstrSql, uSqlLen);
	m_strSQL[uSqlLen - 1] = '\0';

	return true;
}

void CProcObj::Clear()
{
	m_pRequest->uClientID	= 0;
	m_bAddParam				= false;
}
