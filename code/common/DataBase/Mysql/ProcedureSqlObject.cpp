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

	m_bAddColumn			= false;
	m_bAddBatchValue		= false;
	m_bAddParam				= false;
}

CProcObj::~CProcObj()
{
	SAFE_DELETE(m_pstrBuffer);
}

bool CProcObj::Initialize(const uint32_t uQueryBufferLen)
{
	m_uMaxSQLLen	= uQueryBufferLen;
	if (0 == m_uMaxSQLLen)
	{
		g_pFileLog->WriteLog("[%s][%d] SQL Buffer Len[%u] Error\n", __FUNCTION__, __LINE__, m_uMaxSQLLen);
		return false;
	}

	m_wMaxCallbackDataLen	= MAX_CALLBACK_DATA_LEN + sizeof(uint16_t);
	m_uBufferLen			= m_uMaxSQLLen + m_wMaxCallbackDataLen;
	m_pstrBuffer			= new char[m_uBufferLen];
	if (nullptr == m_pstrBuffer)
	{
		g_pFileLog->WriteLog("[%s][%d] new m_strBuffer[%u] Failed\n", __FUNCTION__, __LINE__, m_uBufferLen);
		return false;
	}

	m_pCallbackDataLen	= (uint16_t*)m_pstrBuffer;
	m_pstrCallbackData	= m_pstrBuffer + sizeof(uint16_t);
	m_pstrSQL			= m_pstrBuffer + m_wMaxCallbackDataLen;

	return true;
}

bool CProcObj::BeginBatchInsert(const char *pstrTableName, void *pCallbackData, const uint16_t wDataLen)
{
	if (nullptr == pstrTableName)
		return false;

	if (nullptr == pCallbackData)
		return false;

	if (wDataLen > MAX_CALLBACK_DATA_LEN)
		return false;

	if (m_uSQLLen >= m_uMaxSQLLen)
		return false;

	Clear();

	*m_pCallbackDataLen	= wDataLen;

	memcpy(m_pstrCallbackData, pCallbackData, wDataLen);

	m_uSQLLen += snprintf(m_pstrSQL, m_uMaxSQLLen - m_uSQLLen, "insert into %s(", pstrTableName);

	return true;
}

bool CProcObj::AddColumn(const char *pstrColName)
{
	if (nullptr == pstrColName)
		return false;

	if (m_uMaxSQLLen == m_uSQLLen)
		return false;

	if (m_bAddColumn)
	{
		m_uSQLLen += snprintf(m_pstrSQL + m_uSQLLen, m_uMaxSQLLen - m_uSQLLen, ",%s", pstrColName);
	}
	else
	{
		m_uSQLLen += snprintf(m_pstrSQL + m_uSQLLen, m_uMaxSQLLen - m_uSQLLen, "%s", pstrColName);
	}

	m_bAddColumn = true;

	return true;
}

bool CProcObj::EndColumn()
{
	if (m_uSQLLen >= m_uMaxSQLLen)
		return false;

	m_uSQLLen += snprintf(m_pstrSQL + m_uSQLLen, m_uMaxSQLLen - m_uSQLLen, ") value");

	return true;
}

bool CProcObj::BeginAddParam()
{
	if (m_bAddBatchValue)
	{
		m_uSQLLen += snprintf(m_pstrSQL + m_uSQLLen, m_uMaxSQLLen - m_uSQLLen, ",(");
	}
	else
	{
		m_uSQLLen += snprintf(m_pstrSQL + m_uSQLLen, m_uMaxSQLLen - m_uSQLLen, "(");
	}

	m_bAddBatchValue	= true;

	return true;
}

bool CProcObj::EndAddParam()
{
	if (m_uSQLLen >= m_uMaxSQLLen)
		return false;

	m_uSQLLen += snprintf(m_pstrSQL + m_uSQLLen, m_uMaxSQLLen - m_uSQLLen, ")");

	return true;
}

bool CProcObj::BatchInsert()
{
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

bool CProcObj::AddParam(const int64_t nParam)
{
	if (m_uMaxSQLLen == m_uSQLLen)
		return false;

	if (m_bAddParam)
	{
		m_uSQLLen += snprintf(m_pstrSQL + m_uSQLLen, m_uMaxSQLLen - m_uSQLLen, ",%lld", nParam);
	}
	else
	{
		m_uSQLLen += snprintf(m_pstrSQL + m_uSQLLen, m_uMaxSQLLen - m_uSQLLen, "%lld", nParam);
	}

	m_bAddParam = true;

	return true;
}

bool CProcObj::AddParam(const uint64_t uParam)
{
	if (m_uMaxSQLLen == m_uSQLLen)
		return false;

	if (m_bAddParam)
	{
		m_uSQLLen += snprintf(m_pstrSQL + m_uSQLLen, m_uMaxSQLLen - m_uSQLLen, ",%llu", uParam);
	}
	else
	{
		m_uSQLLen += snprintf(m_pstrSQL + m_uSQLLen, m_uMaxSQLLen - m_uSQLLen, "%llu", uParam);
	}

	m_bAddParam = true;

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

bool CProcObj::AddParam(const uint32_t uParam)
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

bool CProcObj::AddParam(const uint16_t usParam)
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

bool CProcObj::AddParam(const uint8_t byParam)
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
		m_uSQLLen += snprintf(m_pstrSQL + m_uSQLLen, m_uMaxSQLLen - m_uSQLLen, ",%0.8f", dParam);
	}
	else
	{
		m_uSQLLen += snprintf(m_pstrSQL + m_uSQLLen, m_uMaxSQLLen - m_uSQLLen, "%0.8f", dParam);
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

bool CProcObj::AddParam(const void *pParam, const unsigned int uParamLen)
{
	if (nullptr == pParam)
		return false;

	if (m_bAddParam)
	{
		m_uSQLLen += snprintf(m_pstrSQL + m_uSQLLen, m_uMaxSQLLen - m_uSQLLen, ",'");
	}
	else
	{
		m_uSQLLen += snprintf(m_pstrSQL + m_uSQLLen, m_uMaxSQLLen - m_uSQLLen, "'");
	}

	if (m_uMaxSQLLen - m_uSQLLen < uParamLen * 2 + 1)
		return false;

	m_uSQLLen += mysql_real_escape_string(m_pDBHandle, m_pstrSQL + m_uSQLLen, (const char*)pParam, uParamLen);

	m_uSQLLen += snprintf(m_pstrSQL + m_uSQLLen, m_uMaxSQLLen - m_uSQLLen, "'");

	m_bAddParam = true;

	return true;
}

bool CProcObj::EndPrepareProc(void *pCallbackData, const uint16_t wDataLen)
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

	m_bAddColumn		= false;
	m_bAddBatchValue	= false;
	m_bAddParam			= false;
}
