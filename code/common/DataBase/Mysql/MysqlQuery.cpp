#include "stdafx.h"
#include "Daemon.h"
#include "MysqlQuery.h"

#define MAX_ROLE_DATA_LENGTH				4096
#define MAX_MAIL_DATA_LENGTH				409600

IMysqlQuery *CreateMysqlQuery(char *pstrDBIP, char *pstrAccount, char *pstrPassword, char *pstrDBName, unsigned short usDBPort)
{
	CMysqlQuery *pRoleDB	= new CMysqlQuery;
	if (NULL == pRoleDB)
		return NULL;

	if (!pRoleDB->Initialize(pstrDBIP, pstrAccount, pstrPassword, pstrDBName, usDBPort))
	{
		SAFE_DELETE(pRoleDB);
		return NULL;
	}

	return pRoleDB;
}

CMysqlQuery::CMysqlQuery()
{
	m_pFile				= NULL;
	m_pRBRequest	= NULL;
	m_pRBRespond	= NULL;

	m_pDBHandle			= NULL;
	m_pQueryRes			= NULL;
	m_pRow				= NULL;

	m_usDBPort			= 0;
	m_strDBIP.clear();
	m_strUserName.clear();
	m_strPassword.clear();
	m_strDBName.clear();

	memset(m_strSQL, 0, sizeof(m_strSQL));

	m_uLastError		= 0;

	m_uNextPingTime		= 0;
	m_uNextConnectTime	= 0;
	m_uSleepTime		= 0;
	m_uFrame			= 0;

	m_uPingInterval		= 0;
	m_uReconnectInterval= 0;

	m_nTimeNow			= 0;

	m_bRunning			= false;
	m_bExit				= true;
}

CMysqlQuery::~CMysqlQuery()
{
	if (m_pFile)
	{
		m_pFile->Release();
		m_pFile	= NULL;
	}

	if (m_pDBHandle)
	{
		mysql_close(m_pDBHandle);
		m_pDBHandle	= NULL;
	}

	if (m_pRBRequest)
	{
		m_pRBRequest->Release();
		m_pRBRequest	= NULL;
	}

	if (m_pRBRespond)
	{
		m_pRBRespond->Release();
		m_pRBRespond	= NULL;
	}
}

bool CMysqlQuery::Initialize(char *pstrDBIP, char *pstrAccount, char *pstrPassword, char *pstrDBName, unsigned short usDBPort)
{
	m_strDBIP		= pstrDBIP;
	m_strUserName	= pstrAccount;
	m_strPassword	= pstrPassword;
	m_strDBName		= pstrDBName;
	m_usDBPort		= usDBPort;

	m_pRBRequest	= CreateRingBuffer(ROLE_DB_RB_REQUEST_LEN, ROLE_DB_RB_REQUEST_PACK_LEN);
	if (!m_pRBRequest)
	{
		g_pFileLog->WriteLog("%s[%d] Create RingBuffer For RoleDB Recv Failed!\n", __FILE__, __LINE__);
		return false;
	}

	m_pRBRespond	= CreateRingBuffer(ROLE_DB_RB_RESPOND_LEN, ROLE_DB_RB_RESPOND_PACK_LEN);
	if (!m_pRBRespond)
	{
		g_pFileLog->WriteLog("%s[%d] Create RingBuffer For RoleDB Send Failed!\n", __FILE__, __LINE__);
		return false;
	}

	m_pDBHandle = mysql_init(NULL);
	if (NULL == m_pDBHandle)
	{
		g_pFileLog->WriteLog("%s[%d] mysql_init Failed!\n", __FILE__, __LINE__);
		return false;
	}

	g_pFileLog->WriteLog("Connect DB[%s]...\n", m_strDBIP);

	char	cReconnectFlag = 1;
	if (0 != mysql_options(m_pDBHandle, MYSQL_OPT_RECONNECT, &cReconnectFlag))
	{
		m_uLastError			= mysql_errno(m_pDBHandle);
		const char	*pstrError	= mysql_error(m_pDBHandle);

		g_pFileLog->WriteLog("%s[%d] mysql_options Error:[%u][%s]\n", __FILE__, __LINE__, m_uLastError, pstrError);

		return false;
	}

	if (!mysql_real_connect(m_pDBHandle, m_strDBIP.c_str(), m_strUserName.c_str(), m_strPassword.c_str(), "", m_usDBPort, NULL, CLIENT_MULTI_STATEMENTS))
	{
		m_uLastError			= mysql_errno(m_pDBHandle);
		const char	*pstrError	= mysql_error(m_pDBHandle);

		g_pFileLog->WriteLog("%s[%d] mysql_real_connect Error:[%u][%s]\n", __FILE__, __LINE__, m_uLastError, pstrError);

		return false;
	}
	g_pFileLog->WriteLog("Connect To DB[%s] Success!\n", m_strDBIP);

	//if (0 != mysql_autocommit(m_pDBHandle, 0))
	//{
	//	g_pFileLog->WriteLog("%s:%d, mysql_autocommit Close Failed!\n", __FILE__, __LINE__);
	//	return false;
	//}

	if (!CreateDataBase())
	{
		g_pFileLog->WriteLog("%s[%d] Create DataBase Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (0 != mysql_select_db(m_pDBHandle, m_strDBName.c_str()))
	{
		m_uLastError			= mysql_errno(m_pDBHandle);
		const char	*pstrError	= mysql_error(m_pDBHandle);

		g_pFileLog->WriteLog("%s[%d] mysql_select_db Error:[%u]\n[%s]\n", __FILE__, __LINE__, m_uLastError, pstrError);

		return false;
	}

	if (0 != mysql_set_character_set(m_pDBHandle, "utf8"))
	{
		m_uLastError			= mysql_errno(m_pDBHandle);
		const char	*pstrError	= mysql_error(m_pDBHandle);

		g_pFileLog->WriteLog("%s[%d] mysql_set_character_set Error:[%u]\n[%s]\n", __FILE__, __LINE__, m_uLastError, pstrError);

		return false;
	}

	m_bRunning	= true;

	std::thread	threadMysqlQuery(&CMysqlQuery::DBThreadFunc, this);
	threadMysqlQuery.detach();

	return true;
}

bool CMysqlQuery::SendDBRequest(const void *pPack, const unsigned int uPackLen)
{
	return m_pRBRequest->SndPack(pPack, uPackLen);
}

const void *CMysqlQuery::GetDBRespond(unsigned int &uPackLen)
{
	return m_pRBRespond->RcvPack(uPackLen);
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

void CMysqlQuery::SaveAllData()
{
	DBActive();

	ProcessRequest();
}

void CMysqlQuery::Release()
{
	delete this;
}

bool CMysqlQuery::IsExistDB(const char *pstrDBName)
{
	if (!m_pDBHandle)
		return false;

	MYSQL_RES *pResTemp = mysql_list_dbs(m_pDBHandle, pstrDBName);
	if (NULL == pResTemp)
		return false;

	if (mysql_num_rows(pResTemp) > 0)
	{
		mysql_free_result(pResTemp);
		return true;
	}

	mysql_free_result(pResTemp);

	return false;
}

bool CMysqlQuery::Query(const char *pstrSQL, const unsigned int uSQLLen)
{
	if (!m_pDBHandle)
	{
		g_pFileLog->WriteLog("%s[%d] m_pDBHandle Is NULL\n", __FILE__, __LINE__);
		return false;
	}

	if (0 != mysql_real_query(m_pDBHandle, pstrSQL, uSQLLen))	// 执行单个sql查询
	{
		m_uLastError			= mysql_errno(m_pDBHandle);
		const char	*pstrError	= mysql_error(m_pDBHandle);

		g_pFileLog->WriteLog("%s[%d] mysql_real_query Error:[%u]\n[%s]\n", __FILE__, __LINE__, m_uLastError, pstrError);

		return false;
	}

	return true;
}

void CMysqlQuery::Disconnect()
{
	if (m_pDBHandle)
	{
		mysql_close(m_pDBHandle);
		m_pDBHandle	= NULL;
	}
}

bool CMysqlQuery::CreateDataBase()
{
	if (IsExistDB(m_strDBName.c_str()))
	{
		// 数据库已存在，直接返回
		return true;
	}

	int	nStrLen	= snprintf(m_strSQL, sizeof(m_strSQL), "create database if not exists %s CHARACTER SET 'utf8' COLLATE 'utf8_general_ci", m_strDBName);
	if (0 != mysql_real_query(m_pDBHandle, m_strSQL, nStrLen))
	{
		m_uLastError			= mysql_errno(m_pDBHandle);
		const char	*pstrError	= mysql_error(m_pDBHandle);

		g_pFileLog->WriteLog("%s[%d] mysql_real_query Error:[%u]\n[%s]\n", __FILE__, __LINE__, m_uLastError, pstrError);

		return false;
	}

	if (0 != mysql_select_db(m_pDBHandle, m_strDBName.c_str()))
	{
		m_uLastError			= mysql_errno(m_pDBHandle);
		const char	*pstrError	= mysql_error(m_pDBHandle);

		g_pFileLog->WriteLog("%s[%d] mysql_select_db Error:[%u]\n[%s]\n", __FILE__, __LINE__, m_uLastError, pstrError);

		return false;
	}

	if (0 != mysql_set_character_set(m_pDBHandle, "utf8"))
	{
		m_uLastError			= mysql_errno(m_pDBHandle);
		const char	*pstrError	= mysql_error(m_pDBHandle);

		g_pFileLog->WriteLog("%s[%d] mysql_set_character_set Error:[%u]\n[%s]\n", __FILE__, __LINE__, m_uLastError, pstrError);

		return false;
	}

	return true;
}

bool CMysqlQuery::ExecuteSQL(const char *pstrSQL)
{
	int nStrLen	= snprintf(m_strSQL, sizeof(m_strSQL), pstrSQL);

	if (nStrLen <= 0 || nStrLen >= sizeof(m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] nStrLen Is Error:\n[%d]\n", __FILE__, __LINE__, nStrLen);
		return false;
	}

	if (!Query(m_strSQL, nStrLen))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return false;
	}

	return true;
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

	m_pDBHandle	= mysql_init(NULL);
	if (!m_pDBHandle)
	{
		g_pFileLog->WriteLog("%s[%d] m_pDBHandle Is NULL\n", __FILE__, __LINE__);

		m_uNextConnectTime = m_nTimeNow + m_uReconnectInterval;

		return;
	}

	char	cReconnectFlag = 1;
	if (0 != mysql_options(m_pDBHandle, MYSQL_OPT_RECONNECT, &cReconnectFlag))
	{
		m_uLastError			= mysql_errno(m_pDBHandle);
		const char	*pstrError	= mysql_error(m_pDBHandle);

		g_pFileLog->WriteLog("%s[%d] mysql_options Error:\n[%u][%s]\n", __FILE__, __LINE__, m_uLastError, pstrError);

		Disconnect();

		m_uNextConnectTime = m_nTimeNow + m_uReconnectInterval;

		return;
	}

	if (!mysql_real_connect(m_pDBHandle, m_strDBIP.c_str(), m_strUserName.c_str(), m_strPassword.c_str(), m_strDBName.c_str(), m_usDBPort, m_strDBName.c_str(), 0))
	{
		m_uLastError			= mysql_errno(m_pDBHandle);
		const char	*pstrError	= mysql_error(m_pDBHandle);

		g_pFileLog->WriteLog("%s[%d] mysql_real_connect Error:\n[%u][%s]\n", __FILE__, __LINE__, m_uLastError, pstrError);

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
	UINT		uiPackLen	= 0;
	const void	*pPack		= NULL;

	while(NULL != (pPack = m_pRBRequest->RcvPack(uiPackLen)))
	{
		BYTE	byProtocol = *((BYTE*)pPack);

		//if (byProtocol >= role_db_end)
		//{
		//	g_pFileLog->WriteLog("%s[%d] Invalid Protocol[%hhu]\n", __FILE__, __LINE__, byProtocol);
		//	return;
		//}

		//(this->*m_ProtocolFunc[byProtocol])(pPack, uiPackLen);
	};
}
