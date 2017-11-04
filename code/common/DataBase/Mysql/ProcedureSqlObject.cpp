#include "../stdafx.h"
#include "my_global.h"
#include "mysql.h"
#include "ProcedureSqlObject.h"
#include "MysqlQuery.h"

CProcObj::CProcObj()
{
	m_pDBHandle				= nullptr;

	m_pstrBuffer			= nullptr;
	m_pCallbackDataLen		= nullptr;
	m_pstrCallbackData		= nullptr;
	m_pstrSQL				= nullptr;

	m_uBufferLen			= 0;
	m_uMaxSQLLen			= 0;
	m_uSQLLen				= 0;

	m_wMaxCallbackDataLen	= 0;

	m_bAddParam				= false;
}

CProcObj::~CProcObj()
{
	SAFE_DELETE(m_pstrBuffer);
}

bool CProcObj::Initialize(const UINT uQueryBufferLen)
{
	m_uMaxSQLLen	= uQueryBufferLen;
	if (0 == m_uMaxSQLLen)
	{
		g_pFileLog->WriteLog("[%s][%d] SQL Buffer Len[%u] Error\n", __FUNCTION__, __LINE__, m_uMaxSQLLen);
		return false;
	}

	m_wMaxCallbackDataLen	= MAX_CALLBACK_DATA_LEN + sizeof(WORD);
	m_uBufferLen			= m_uMaxSQLLen + m_wMaxCallbackDataLen;
	m_pstrBuffer			= new char[m_uBufferLen];
	if (nullptr == m_pstrBuffer)
	{
		g_pFileLog->WriteLog("[%s][%d] new m_strBuffer[%u] Failed\n", __FUNCTION__, __LINE__, m_uBufferLen);
		return false;
	}

	m_pCallbackDataLen	= (WORD*)m_pstrBuffer;
	m_pstrCallbackData	= m_pstrBuffer + sizeof(WORD);
	m_pstrSQL			= m_pstrBuffer + m_wMaxCallbackDataLen;

	return true;
}

bool CProcObj::PrepareProc(const char *pstrProcName)
{
	if (nullptr == pstrProcName)
		return false;

	Clear();

	m_uSQLLen += snprintf(m_pstrSQL, m_uMaxSQLLen - m_uSQLLen, "CALL %s (", pstrProcName);

	return true;
}

bool CProcObj::AddParam(const int nParam)
{
	if (m_uMaxSQLLen == m_uSQLLen)
		return false;

	if (m_bAddParam)
	{
		m_uSQLLen += snprintf(m_pstrSQL + m_uSQLLen, m_uMaxSQLLen - m_uSQLLen, ",%d", nParam);
	}
	else
	{
		m_uSQLLen += snprintf(m_pstrSQL + m_uSQLLen, m_uMaxSQLLen - m_uSQLLen, "%d", nParam);
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
		m_uSQLLen += snprintf(m_pstrSQL + m_uSQLLen, m_uMaxSQLLen - m_uSQLLen, ",%u", uParam);
	}
	else
	{
		m_uSQLLen += snprintf(m_pstrSQL + m_uSQLLen, m_uMaxSQLLen - m_uSQLLen, "%u", uParam);
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
		m_uSQLLen += snprintf(m_pstrSQL + m_uSQLLen, m_uMaxSQLLen - m_uSQLLen, ",%hd", sParam);
	}
	else
	{
		m_uSQLLen += snprintf(m_pstrSQL + m_uSQLLen, m_uMaxSQLLen - m_uSQLLen, "%hd", sParam);
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
		m_uSQLLen += snprintf(m_pstrSQL + m_uSQLLen, m_uMaxSQLLen - m_uSQLLen, ",%hu", usParam);
	}
	else
	{
		m_uSQLLen += snprintf(m_pstrSQL + m_uSQLLen, m_uMaxSQLLen - m_uSQLLen, "%hu", usParam);
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
		m_uSQLLen += snprintf(m_pstrSQL + m_uSQLLen, m_uMaxSQLLen - m_uSQLLen, ",%hhu", byParam);
	}
	else
	{
		m_uSQLLen += snprintf(m_pstrSQL + m_uSQLLen, m_uMaxSQLLen - m_uSQLLen, "%hhu", byParam);
	}

	m_bAddParam = true;

	return true;
}

bool CProcObj::AddParam(const float fParam)
{
	if (m_uMaxSQLLen == m_uSQLLen)
		return false;

	if (m_bAddParam)
	{
		m_uSQLLen += snprintf(m_pstrSQL + m_uSQLLen, m_uMaxSQLLen - m_uSQLLen, ",%f", fParam);
	}
	else
	{
		m_uSQLLen += snprintf(m_pstrSQL + m_uSQLLen, m_uMaxSQLLen - m_uSQLLen, "%f", fParam);
	}

	m_bAddParam = true;

	return true;
}

bool CProcObj::AddParam(const double dParam)
{
	if (m_uMaxSQLLen == m_uSQLLen)
		return false;

	if (m_bAddParam)
	{
		m_uSQLLen += snprintf(m_pstrSQL + m_uSQLLen, m_uMaxSQLLen - m_uSQLLen, ",%f", dParam);
	}
	else
	{
		m_uSQLLen += snprintf(m_pstrSQL + m_uSQLLen, m_uMaxSQLLen - m_uSQLLen, "%f", dParam);
	}

	m_bAddParam = true;

	return true;
}

bool CProcObj::AddParam(const char *pstrParam)
{
	if (nullptr == pstrParam)
		return false;

	if (m_bAddParam)
	{
		m_uSQLLen += snprintf(m_pstrSQL + m_uSQLLen, m_uMaxSQLLen - m_uSQLLen, ",'");
	}
	else
	{
		m_uSQLLen += snprintf(m_pstrSQL + m_uSQLLen, m_uMaxSQLLen - m_uSQLLen, "'");
	}

	size_t	nParamLen	= strlen(pstrParam);

	if (m_uMaxSQLLen - m_uSQLLen < nParamLen * 2 + 1)
		return false;

	m_uSQLLen += mysql_real_escape_string(m_pDBHandle, m_pstrSQL + m_uSQLLen, pstrParam, nParamLen);

	m_uSQLLen += snprintf(m_pstrSQL + m_uSQLLen, m_uMaxSQLLen - m_uSQLLen, "'");

	m_bAddParam = true;

	return true;
}

bool CProcObj::AddParam(const void *pParam)
{
	return true;
}

bool CProcObj::AddLikeParam(const char *pstrParam)
{
	if (nullptr == pstrParam)
		return false;

	if (m_bAddParam)
	{
		m_uSQLLen += snprintf(m_pstrSQL + m_uSQLLen, m_uMaxSQLLen - m_uSQLLen, ",'%");
	}
	else
	{
		m_uSQLLen += snprintf(m_pstrSQL + m_uSQLLen, m_uMaxSQLLen - m_uSQLLen, "'%");
	}

	size_t	nParamLen = strlen(pstrParam);

	if (m_uMaxSQLLen - m_uSQLLen < nParamLen * 2 + 1)
		return false;

	m_uSQLLen += mysql_real_escape_string(m_pDBHandle, m_pstrSQL + m_uSQLLen, pstrParam, nParamLen);

	m_uSQLLen += snprintf(m_pstrSQL + m_uSQLLen, m_uMaxSQLLen - m_uSQLLen, "%'");

	m_bAddParam = true;

	return true;
}

bool CProcObj::EndPrepareProc(void *pCallbackData, const WORD wDataLen)
{
	if (wDataLen > MAX_CALLBACK_DATA_LEN)
		return false;

	if (m_uSQLLen >= m_uMaxSQLLen)
		return false;

	*m_pCallbackDataLen	= wDataLen;

	m_uSQLLen += snprintf(m_pstrSQL + m_uSQLLen, m_uMaxSQLLen - m_uSQLLen, ");");

	memcpy(m_pstrCallbackData, pCallbackData, wDataLen);

	return true;
}

void CProcObj::Clear()
{
	*m_pCallbackDataLen	= 0;
	m_uSQLLen			= 0;
	m_bAddParam			= false;
}
