#include "stdafx.h"
#include "Daemon.h"
#include "my_global.h"
#include "mysql.h"
#include "MysqlQuery.h"
#include "ProcedureSqlObject.h"
#include "MysqlResult.h"
#include "MysqlResultSet.h"

IMysqlQuery *CreateMysqlQuery(const char *pstrSettingFile, const char *pstrSection)
{
	CMysqlQuery *pMysqlQuery	= new CMysqlQuery;
	if (nullptr == pMysqlQuery)
		return nullptr;

	if (!pMysqlQuery->Initialize(pstrSettingFile, pstrSection))
	{
		SAFE_DELETE(pMysqlQuery);
		return nullptr;
	}

	return pMysqlQuery;
}

CMysqlQuery::CMysqlQuery()
{
	m_pIniFile			= nullptr;

	m_pRBRequest		= nullptr;
	m_pRBRespond		= nullptr;

	m_pProcSqlObj		= nullptr;
	m_pResultSet		= nullptr;

	m_pDBHandle			= nullptr;
	m_pQueryRes			= nullptr;
	m_pRow				= nullptr;
	m_pResultBuffer		= nullptr;
	m_pCurPos			= nullptr;
	m_pResultSetHead	= nullptr;
	m_pResultHead		= nullptr;
	m_pDataHead			= nullptr;

	m_uSqlBufferLen		= 0;
	m_uMaxSqlLen		= 0;
	m_uResultBufferLen	= 0;
	m_uMaxResultLen		= 0;

	m_uLeftBufferLen	= 0;
	m_byMaxResultCount	= 0;

	m_uNextPingTime		= 0;
	m_uNextConnectTime	= 0;
	m_uSleepTime		= 0;
	m_uFrame			= 0;

	m_uPingInterval		= 0;
	m_uReconnectInterval= 0;

	m_nTimeNow			= 0;

	m_usDBPort			= 0;
	m_strDBIP.clear();
	m_strUserName.clear();
	m_strPassword.clear();
	m_strDBName.clear();
	m_strCharacterSet.clear();

	m_vectMysqlRes.clear();

	m_bRunning			= false;
	m_bExit				= true;
}

CMysqlQuery::~CMysqlQuery()
{
	if (m_pIniFile)
	{
		m_pIniFile->Release();
		m_pIniFile	= nullptr;
	}

	if (m_pRBRequest)
	{
		m_pRBRequest->Release();
		m_pRBRequest	= nullptr;
	}

	if (m_pRBRespond)
	{
		m_pRBRespond->Release();
		m_pRBRespond	= nullptr;
	}

	SAFE_DELETE(m_pProcSqlObj);
	SAFE_DELETE(m_pResultSet);

	if (m_pDBHandle)
	{
		mysql_close(m_pDBHandle);
		m_pDBHandle	= nullptr;
	}

	SAFE_DELETE_ARR(m_pResultBuffer);
}

bool CMysqlQuery::Initialize(const char *pstrSettingFile, const char *pstrSection)
{
	if (!LoadConfig(pstrSettingFile, pstrSection))
	{
		g_pFileLog->WriteLog("[%s][%d] Load Config[%s] Section[%s] Failed\n", __FUNCTION__, __LINE__, pstrSettingFile, pstrSection);
		return false;
	}

	m_pRBRequest	= CreateRingBuffer(m_uSqlBufferLen, m_uMaxSqlLen);
	if (!m_pRBRequest)
	{
		g_pFileLog->WriteLog("%s[%d] Create RingBuffer For RoleDB Recv Failed\n", __FILE__, __LINE__);
		return false;
	}

	m_pRBRespond = CreateRingBuffer(m_uResultBufferLen*m_byMaxResultCount, m_uMaxResultLen);
	if (!m_pRBRespond)
	{
		g_pFileLog->WriteLog("%s[%d] Create RingBuffer For RoleDB Send Failed\n", __FILE__, __LINE__);
		return false;
	}

	m_pProcSqlObj	= new CProcObj;
	if (nullptr == m_pProcSqlObj)
	{
		g_pFileLog->WriteLog("[%s][%d] new CProcSqlObj Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!m_pProcSqlObj->Initialize(m_uSqlBufferLen, *m_pDBHandle))
	{
		g_pFileLog->WriteLog("[%s][%d] CProcSqlObj Initialize Failed\n", __FILE__, __LINE__);
		return false;
	}

	m_pResultSet	= new CMysqlResultSet(*this);
	if (nullptr == m_pResultSet)
	{
		g_pFileLog->WriteLog("[%s][%d] new CMysqlResultSet Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!m_pResultSet->Initialize(m_byMaxResultCount))
	{
		g_pFileLog->WriteLog("[%s][%d] CMysqlResultSet::Initialize Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (m_uResultBufferLen*m_byMaxResultCount < sizeof(SResultSetHead))
	{
		g_pFileLog->WriteLog("[%s][%d] CMysqlResultSet::Initialize Result Buffer[%u] Not Enough\n", __FILE__, __LINE__, m_uResultBufferLen*m_byMaxResultCount);
		return false;
	}

	m_pResultBuffer = new char[m_uResultBufferLen*m_byMaxResultCount];
	if (nullptr == m_pResultBuffer)
	{
		g_pFileLog->WriteLog("[%s][%d] new char[%u] For Result Buffer Failed\n", __FILE__, __LINE__, m_uResultBufferLen*m_byMaxResultCount);
		return false;
	}

	m_pResultSetHead	= (SResultSetHead *)m_pResultBuffer;

	m_pDBHandle = mysql_init(nullptr);
	if (nullptr == m_pDBHandle)
	{
		g_pFileLog->WriteLog("%s[%d] mysql_init Failed\n", __FILE__, __LINE__);
		return false;
	}

	g_pFileLog->WriteLog("Connect DB[%s]...\n", m_strDBIP.c_str());

	char	cReconnectFlag = 1;
	if (0 != mysql_options(m_pDBHandle, MYSQL_OPT_RECONNECT, &cReconnectFlag))
	{
		unsigned int	uLastError	= mysql_errno(m_pDBHandle);
		const char		*pstrError	= mysql_error(m_pDBHandle);

		g_pFileLog->WriteLog("%s[%d] mysql_options Error:[%u][%s]\n", __FILE__, __LINE__, uLastError, pstrError);

		return false;
	}

	if (!mysql_real_connect(m_pDBHandle, m_strDBIP.c_str(), m_strUserName.c_str(), m_strPassword.c_str(), "", m_usDBPort, nullptr, CLIENT_MULTI_STATEMENTS))
	{
		unsigned int	uLastError	= mysql_errno(m_pDBHandle);
		const char		*pstrError	= mysql_error(m_pDBHandle);

		g_pFileLog->WriteLog("%s[%d] mysql_real_connect Error:[%u][%s]\n", __FILE__, __LINE__, uLastError, pstrError);

		return false;
	}
	g_pFileLog->WriteLog("Connect To DB[%s] Success\n", m_strDBIP.c_str());

	//if (0 != mysql_autocommit(m_pDBHandle, 0))
	//{
	//	g_pFileLog->WriteLog("%s:%d, mysql_autocommit Close Failed!\n", __FILE__, __LINE__);
	//	return false;
	//}

	if (0 != mysql_select_db(m_pDBHandle, m_strDBName.c_str()))
	{
		unsigned int	uLastError	= mysql_errno(m_pDBHandle);
		const char		*pstrError	= mysql_error(m_pDBHandle);

		g_pFileLog->WriteLog("%s[%d] mysql_select_db Error:[%u]\n[%s]\n", __FILE__, __LINE__, uLastError, pstrError);

		return false;
	}

	if (0 != mysql_set_character_set(m_pDBHandle, m_strCharacterSet.c_str()))
	{
		unsigned int	uLastError	= mysql_errno(m_pDBHandle);
		const char		*pstrError	= mysql_error(m_pDBHandle);

		g_pFileLog->WriteLog("%s[%d] mysql_set_character_set Error:[%u]\n[%s]\n", __FILE__, __LINE__, uLastError, pstrError);

		return false;
	}

	m_bRunning	= true;

	std::thread	threadMysqlQuery(&CMysqlQuery::DBThreadFunc, this);
	threadMysqlQuery.detach();

	return true;
}

bool CMysqlQuery::PrepareProc(const char *pstrProcName)
{
	return m_pProcSqlObj->PrepareProc(pstrProcName);
}

bool CMysqlQuery::AddParam(const int nParam)
{
	return m_pProcSqlObj->AddParam(nParam);
}

bool CMysqlQuery::AddParam(const unsigned int uParam)
{
	return m_pProcSqlObj->AddParam(uParam);
}

bool CMysqlQuery::AddParam(const short sParam)
{
	return m_pProcSqlObj->AddParam(sParam);
}

bool CMysqlQuery::AddParam(const unsigned short usParam)
{
	return m_pProcSqlObj->AddParam(usParam);
}

bool CMysqlQuery::AddParam(const unsigned char byParam)
{
	return m_pProcSqlObj->AddParam(byParam);
}

bool CMysqlQuery::AddParam(const char *pstrParam)
{
	return m_pProcSqlObj->AddParam(pstrParam);
}

bool CMysqlQuery::AddParam(const void *pParam)
{
	return m_pProcSqlObj->AddParam(pParam);
}

bool CMysqlQuery::EndPrepareProc(void *pCallbackData, const WORD wDataLen)
{
	return m_pProcSqlObj->EndPrepareProc(pCallbackData, wDataLen);
}

bool CMysqlQuery::CallProc()
{
	UINT		uPackLen	= 0;
	const void	*pPack		= m_pProcSqlObj->GetRequest(uPackLen);

	return m_pRBRequest->SndPack(pPack, uPackLen);
}

const void *CMysqlQuery::GetDBRespond(unsigned int &uPackLen)
{
	return m_pRBRespond->RcvPack(uPackLen);
}

IMysqlResultSet *CMysqlQuery::GetMysqlResultSet()
{
	UINT		uLen		= 0;
	const void	*pRespond	= m_pRBRespond->RcvPack(uLen);

	return (m_pResultSet->ParseResult(pRespond, uLen) ? m_pResultSet : nullptr);
}

void CMysqlQuery::Release()
{
	delete this;
}

bool CMysqlQuery::LoadConfig(const char *pstrSettingFile, const char *pstrSection)
{
	m_pIniFile	= OpenIniFile(pstrSettingFile);
	if (nullptr == m_pIniFile)
	{
		g_pFileLog->WriteLog("%s[%d] Open File[%s] Failed\n", __FILE__, __LINE__, pstrSettingFile);
		return false;
	}

	char	strDBIP[0xf];
	char	strUserName[0xf];
	char	strPassword[0xf];
	char	strDBName[0xf];
	char	strCharacterSet[0xf];
	int		nPingTime;
	int		nSqlBufferLen;
	int		nMaxSqlLen;
	int		nResultBufferLen;
	int		nMaxResultLen;
	int		nMaxResultCount;
	int		nFrame;

	m_pIniFile->GetString(pstrSection, "IP", "", strDBIP, sizeof(strDBIP));
	strDBIP[sizeof(strDBIP)-1]	= '\0';
	m_strDBIP	= strDBIP;

	m_pIniFile->GetString(pstrSection, "Account", "", strUserName, sizeof(strUserName));
	strUserName[sizeof(strUserName)-1]	= '\0';
	m_strUserName	= strUserName;

	m_pIniFile->GetString(pstrSection, "Password", "", strPassword, sizeof(strPassword));
	strPassword[sizeof(strPassword)-1]	= '\0';
	m_strPassword	= strPassword;

	m_pIniFile->GetString(pstrSection, "DBName", "", strDBName, sizeof(strDBName));
	strDBName[sizeof(strDBName)-1]	= '\0';
	m_strDBName	= strDBName;

	m_pIniFile->GetString(pstrSection, "CharacterSet", "", strCharacterSet, sizeof(strCharacterSet));
	strCharacterSet[sizeof(strCharacterSet)-1]	= '\0';
	m_strCharacterSet	= strCharacterSet;

	m_pIniFile->GetInteger(pstrSection, "PingTime", 0, &nPingTime);
	m_uPingInterval	= nPingTime;

	m_pIniFile->GetInteger(pstrSection, "SqlBufferLen", 0, &nSqlBufferLen);
	m_uSqlBufferLen	= nSqlBufferLen;

	m_pIniFile->GetInteger(pstrSection, "MaxSqlLen", 0, &nMaxSqlLen);
	m_uMaxSqlLen	= nMaxSqlLen;

	m_pIniFile->GetInteger(pstrSection, "ResultBufferLen", 0, &nResultBufferLen);
	m_uResultBufferLen	= nResultBufferLen;

	m_pIniFile->GetInteger(pstrSection, "MaxResultLen", 0, &nMaxResultLen);
	m_uMaxResultLen	= nMaxResultLen;

	m_pIniFile->GetInteger(pstrSection, "MaxResultCount", 0, &nMaxResultCount);
	m_byMaxResultCount = nMaxResultCount;

	m_pIniFile->GetInteger(pstrSection, "Frame", 0, &nFrame);
	m_uFrame	= nFrame;

	return true;
}

void CMysqlQuery::DBThreadFunc()
{
	uint64	ullBeginTick		= GetMicroTick();
	uint64	ullNextFrameTick	= 0;
	uint64	ullTickNow			= 0;
	uint64	ullDBThreadFrame	= 0;

	m_bExit	= false;

	while (m_bRunning)
	{
		m_nTimeNow = time(nullptr);
		ullTickNow	= GetMicroTick();

		if (ullTickNow < ullNextFrameTick)
		{
			yield();
			continue;
		}

		++ullDBThreadFrame;

		ullNextFrameTick = ullBeginTick + ullDBThreadFrame * 1000 / m_uFrame;

		DBActive();

		ProcessRequest();
	}

	m_bExit	= true;
}

void CMysqlQuery::DBActive()
{
	if (m_nTimeNow > m_uNextPingTime)
	{
		if (m_pDBHandle)
		{
			mysql_ping(m_pDBHandle);
		}

		m_uNextPingTime = m_nTimeNow + m_uPingInterval;
	}

	if (m_pDBHandle)
		return;

	if (m_nTimeNow < m_uNextConnectTime)
		return;

	g_pFileLog->WriteLog("Reconnect To Role DB...\n");

	m_pDBHandle	= mysql_init(nullptr);
	if (!m_pDBHandle)
	{
		g_pFileLog->WriteLog("%s[%d] m_pDBHandle Is nullptr\n", __FILE__, __LINE__);

		m_uNextConnectTime = m_nTimeNow + m_uReconnectInterval;

		return;
	}

	char	cReconnectFlag = 1;
	if (0 != mysql_options(m_pDBHandle, MYSQL_OPT_RECONNECT, &cReconnectFlag))
	{
		unsigned int	uLastError	= mysql_errno(m_pDBHandle);
		const char		*pstrError	= mysql_error(m_pDBHandle);

		g_pFileLog->WriteLog("%s[%d] mysql_options Error:\n[%u][%s]\n", __FILE__, __LINE__, uLastError, pstrError);

		Disconnect();

		m_uNextConnectTime = m_nTimeNow + m_uReconnectInterval;

		return;
	}

	if (!mysql_real_connect(m_pDBHandle, m_strDBIP.c_str(), m_strUserName.c_str(), m_strPassword.c_str(), m_strDBName.c_str(), m_usDBPort, m_strDBName.c_str(), 0))
	{
		unsigned int	uLastError	= mysql_errno(m_pDBHandle);
		const char		*pstrError	= mysql_error(m_pDBHandle);

		g_pFileLog->WriteLog("%s[%d] mysql_real_connect Error:\n[%u][%s]\n", __FILE__, __LINE__, uLastError, pstrError);

		Disconnect();

		m_uNextConnectTime = m_nTimeNow + m_uReconnectInterval;

		return;
	}

	//if (0 != mysql_autocommit(m_pDBHandle, 0))
	//{
	//	g_pFileLog->WriteLog("%s:%d, mysql_autocommit Close Failed!\n", __FILE__, __LINE__);

	//	Disconnect();

	//	m_uiNextConnectTime	= g_nCenterServerSecond + 10;

	//}

	m_uNextConnectTime = m_nTimeNow + m_uReconnectInterval;
}

void CMysqlQuery::ProcessRequest()
{
	UINT		uPackLen	= 0;
	const void	*pPack		= nullptr;

	while(nullptr != (pPack = m_pRBRequest->RcvPack(uPackLen)))
	{
		ExecuteSQL(pPack, uPackLen);
	};
}

void CMysqlQuery::ExecuteSQL(const void *pPack, const unsigned int uPackLen)
{
	if (!m_pDBHandle)
	{
		g_pFileLog->WriteLog("%s[%d] m_pDBHandle Is nullptr\n", __FUNCTION__, __LINE__);
		return;
	}

	WORD		*pCallbackDataLen	= (WORD*)pPack;
	const void	*pCallbackData		= (char*)pPack + sizeof(WORD);
	char		*pstrSQL			= (char*)pPack + sizeof(WORD) + MAX_CALLBACK_DATA_LEN;
	UINT		uSQLLen				= uPackLen - sizeof(WORD) + MAX_CALLBACK_DATA_LEN;

	if (0 != mysql_real_query(m_pDBHandle, pstrSQL, uSQLLen))
	{
		unsigned int	uLastError	= mysql_errno(m_pDBHandle);
		const char		*pstrError	= mysql_error(m_pDBHandle);

		g_pFileLog->WriteLog("%s[%d] mysql_real_query Error:[%u]\n[%s]\n", __FUNCTION__, __LINE__, uLastError, pstrError);

		return;
	}

	HandleResult(pCallbackData, *pCallbackDataLen);

	ClearResult();
}

void CMysqlQuery::ClearResult()
{
	m_uLeftBufferLen	= m_uResultBufferLen * m_byMaxResultCount;
	m_pCurPos			= m_pResultBuffer;
	m_pResultHead		= nullptr;
	m_pDataHead			= nullptr;

	if (m_pQueryRes)
	{
		mysql_free_result(m_pQueryRes);

		while (!mysql_next_result(m_pDBHandle))
		{
			mysql_free_result(mysql_store_result(m_pDBHandle));
		}
	}
}

bool CMysqlQuery::HandleResult(const void *pCallbackData, const WORD wDataLen)
{
	m_pResultSetHead->wCallBackDataLen	= wDataLen;
	m_pResultSetHead->byResultCount		= 0;
	memcpy(m_pResultSetHead->strCallBackDta, pCallbackData, wDataLen);

	m_pCurPos			+= sizeof(SResultSetHead);
	m_uLeftBufferLen	-= sizeof(SResultSetHead);

	m_pQueryRes = mysql_store_result(m_pDBHandle);
	if (nullptr == m_pQueryRes)
	{
		ClearResult();
		return false;
	}

	if (!AddResult())
	{
		ClearResult();

		return false;
	}

	while (!mysql_next_result(m_pDBHandle))
	{
		MYSQL_RES	*m_pQueryRes = mysql_store_result(m_pDBHandle);
		if (nullptr == m_pQueryRes)
			continue;

		if (m_pResultSetHead->byResultCount >= m_byMaxResultCount)
		{
			ClearResult();
			return false;
		}

		if (!AddResult())
		{
			ClearResult();
			return false;
		}

		++m_pResultSetHead->byResultCount;
	}

	return m_pRBRespond->SndPack(m_pResultBuffer, m_pResultBuffer - m_pCurPos);
}

bool CMysqlQuery::AddResult()
{
	if (nullptr == m_pQueryRes)
	{
		unsigned int	uLastError = mysql_errno(m_pDBHandle);
		const char		*pstrError = mysql_error(m_pDBHandle);

		g_pFileLog->WriteLog("%s[%d] mysql_store_result Error:[%u]\n[%s]\n", __FUNCTION__, __LINE__, uLastError, pstrError);

		return false;
	}

	if (m_uLeftBufferLen < sizeof(SResultHead))
		return false;

	m_pResultHead				= (SResultHead*)m_pCurPos;
	m_pResultHead->uRowCount	= mysql_num_rows(m_pQueryRes);
	m_pResultHead->uColCount	= mysql_num_fields(m_pQueryRes);
	m_pResultHead->uLen			= sizeof(SResultHead);

	m_pCurPos					+= sizeof(SResultHead);
	m_uLeftBufferLen			-= sizeof(SResultHead);

	auto	uTotalHeadLen		= sizeof(SMysqlDataHead)* m_pResultHead->uRowCount * m_pResultHead->uColCount;
	if (m_uLeftBufferLen < uTotalHeadLen)
		return false;

	m_pDataHead					= (SMysqlDataHead*)m_pCurPos;

	m_pResultHead->uLen			+= uTotalHeadLen;

	m_pCurPos					+= uTotalHeadLen;
	m_uLeftBufferLen			-= uTotalHeadLen;

	UINT	uRowIndex	= 0;
	UINT	uOffset		= 0;

	while (nullptr != (m_pRow = mysql_fetch_row(m_pQueryRes)))
	{
		unsigned long	*pRowLength = mysql_fetch_lengths(m_pQueryRes);
		if (!pRowLength)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_fetch_lengths Failed\n", __FILE__, __LINE__);
			return false;
		}

		for (auto uCol = 0; uCol < m_uColCount; ++uCol)
		{
			if (!AddResultData(uRowIndex, uCol, m_pRow[uCol], pRowLength[uCol], uOffset))
			{
				ClearResult();
				return false;
			}
		}

		++uRowIndex;
	}

	return true;
}

bool CMysqlQuery::AddResultData(const UINT uRow, const UINT uCol, const void *pData, const UINT uDataLen, UINT &uOffset)
{
	if (m_uLeftBufferLen < uDataLen)
		return false;

	auto	nIndex = m_pResultHead->uColCount * uRow + uCol;
	m_pDataHead[nIndex].uDataLen	= uDataLen;
	m_pDataHead[nIndex].uOffset		= uOffset;//m_pCurPos - m_pResultBuffer;

	memcpy(m_pCurPos, pData, uDataLen);

	m_pCurPos			+= uDataLen;
	m_uLeftBufferLen	-= uDataLen;
	m_pResultHead->uLen	+= uDataLen;
	uOffset				+= uDataLen;

	return true;
}

void CMysqlQuery::Disconnect()
{
	ClearResult();

	if (m_pDBHandle)
	{
		mysql_close(m_pDBHandle);
		m_pDBHandle	= nullptr;
	}
}
