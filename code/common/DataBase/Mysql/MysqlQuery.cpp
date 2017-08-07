#include "stdafx.h"
#include "Daemon.h"
#include "MysqlQuery.h"

IMysqlQuery *CreateMysqlQuery(char *pstrDBIP, char *pstrAccount, char *pstrPassword, char *pstrDBName, unsigned short usDBPort, char *pstrCharset, unsigned int uPingTime)
{
	CMysqlQuery *pMysqlQuery	= new CMysqlQuery;
	if (nullptr == pMysqlQuery)
		return nullptr;

	if (!pMysqlQuery->Initialize(pstrDBIP, pstrAccount, pstrPassword, pstrDBName, usDBPort, pstrCharset, uPingTime))
	{
		SAFE_DELETE(pMysqlQuery);
		return nullptr;
	}

	return pMysqlQuery;
}

CMysqlQuery::CMysqlQuery()
{
	m_pIniFile		= nullptr;

	m_pRBRequest	= nullptr;
	m_pRBRespond	= nullptr;

	m_pDBHandle		= nullptr;
	m_pQueryRes		= nullptr;
	m_pRow			= nullptr;

	m_usDBPort		= 0;
	m_strDBIP.clear();
	m_strUserName.clear();
	m_strPassword.clear();
	m_strDBName.clear();
	m_strCharacterSet.clear();

	m_uSqlBufferLen		= 0;
	m_uMaxSqlLen		= 0;
	m_uResultBufferLen	= 0;
	m_uMaxResultLen		= 0;

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
	if (m_pIniFile)
	{
		m_pIniFile->Release();
		m_pIniFile	= nullptr;
	}

	if (m_pDBHandle)
	{
		mysql_close(m_pDBHandle);
		m_pDBHandle	= nullptr;
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
}

bool CMysqlQuery::Initialize(
							char *pstrDBIP,
							char *pstrAccount,
							char *pstrPassword,
							char *pstrDBName,
							unsigned short usDBPort,
							char *pstrCharset,
							unsigned int uPingTime
							)
{
	m_strDBIP			= pstrDBIP;
	m_strUserName		= pstrAccount;
	m_strPassword		= pstrPassword;
	m_strDBName			= pstrDBName;
	m_usDBPort			= usDBPort;
	m_strCharacterSet	= pstrCharset;
	m_uPingInterval		= uPingTime;

	m_pRBRequest	= CreateRingBuffer(m_uSqlBufferLen, m_uMaxSqlLen);
	if (!m_pRBRequest)
	{
		g_pFileLog->WriteLog("%s[%d] Create RingBuffer For RoleDB Recv Failed!\n", __FILE__, __LINE__);
		return false;
	}

	m_pRBRespond	= CreateRingBuffer(m_uResultBufferLen, m_uMaxResultLen);
	if (!m_pRBRespond)
	{
		g_pFileLog->WriteLog("%s[%d] Create RingBuffer For RoleDB Send Failed!\n", __FILE__, __LINE__);
		return false;
	}

	m_pDBHandle = mysql_init(nullptr);
	if (nullptr == m_pDBHandle)
	{
		g_pFileLog->WriteLog("%s[%d] mysql_init Failed!\n", __FILE__, __LINE__);
		return false;
	}

	g_pFileLog->WriteLog("Connect DB[%s]...\n", m_strDBIP);

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
	g_pFileLog->WriteLog("Connect To DB[%s] Success!\n", m_strDBIP);

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

bool CMysqlQuery::Initialize(char *pstrSettingFile, char *pstrSection)
{
	if (!LoadConfig(pstrSection))
		return false;

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

void CMysqlQuery::Release()
{
	delete this;
}

bool CMysqlQuery::LoadConfig(char *pstrSection)
{
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
		ExecuteSQL((const char *)pPack, uPackLen);
	};
}

bool CMysqlQuery::ExecuteSQL(const char *pstrSQL, const unsigned int uSQLLen)
{
	if (!m_pDBHandle)
	{
		g_pFileLog->WriteLog("%s[%d] m_pDBHandle Is nullptr\n", __FUNCTION__, __LINE__);
		return false;
	}

	if (0 != mysql_real_query(m_pDBHandle, pstrSQL, uSQLLen))
	{
		unsigned int	uLastError	= mysql_errno(m_pDBHandle);
		const char		*pstrError	= mysql_error(m_pDBHandle);

		g_pFileLog->WriteLog("%s[%d] mysql_real_query Error:[%u]\n[%s]\n", __FUNCTION__, __LINE__, uLastError, pstrError);

		return false;
	}

	return true;
}

void CMysqlQuery::Disconnect()
{
	if (m_pDBHandle)
	{
		mysql_close(m_pDBHandle);
		m_pDBHandle	= nullptr;
	}
}
