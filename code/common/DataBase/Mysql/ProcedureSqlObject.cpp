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
	m_uRequestLen	= 0;

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
}

bool CProcObj::AddParam(const int nParam)
{
	return true;
}

bool CProcObj::AddParam(const unsigned int uParam)
{
	return true;
}

bool CProcObj::AddParam(const short sParam)
{
	return true;
}

bool CProcObj::AddParam(const unsigned short usParam)
{
	return true;
}

bool CProcObj::AddParam(const char cParam)
{
	return true;
}

bool CProcObj::AddParam(const unsigned char byParam)
{
	return true;
}

bool CProcObj::AddParam(const char *pstrParam)
{
	return true;
}

bool CProcObj::AddParam(const void *pParam)
{
	return true;
}

bool CProcObj::EndPrepareProc(SMysqlRequest &tagRequest)
{
	return true;
}

bool CProcObj::CallProc()
{
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

	m_uRequestLen		= uPackLen;
	char	*pstrSql	= (char*)pPack + sizeof(SMysqlRequest);
	memcpy(m_pRequest, pPack, sizeof(SMysqlRequest));
	strncpy(m_strSQL, pstrSql, uSqlLen);
	m_strSQL[uSqlLen - 1] = '\0';

	return true;
}

void CProcObj::Clear()
{
	m_pRequest->wSqlLen		= 0;
	m_pRequest->uClientID	= 0;
	m_bAddParam				= false;
}
