#include "stdafx.h"
#include "MysqlQuery.h"

#define MAX_ROLE_DATA_LENGTH				4096
#define MAX_MAIL_DATA_LENGTH				409600

IRoleDB *CreateRoleDB(char *pstrDBIP, char *pstrAccount, char *pstrPassword, char *pstrDBName, unsigned short usDBPort)
{
	CRoleDBThread *pRoleDB	= new CRoleDBThread;
	if (NULL == pRoleDB)
		return NULL;

	if (!pRoleDB->Initialize(pstrDBIP, pstrAccount, pstrPassword, pstrDBName, usDBPort))
	{
		SAFE_DELETE(pRoleDB);
		return NULL;
	}

	return pRoleDB;
}

CRoleDBThread::pfnProtocolFunc CRoleDBThread::m_ProtocolFunc[role_db_end];

CRoleDBThread::CRoleDBThread()
{
	m_pFile				= NULL;
	m_pRBRoleDBRequest	= NULL;
	m_pRBRoleDBRespond	= NULL;

	m_pDBHandle			= NULL;
	m_pQueryRes			= NULL;
	m_pRow				= NULL;

	memset(m_strDBIP, 0, sizeof(m_strDBIP));
	m_usDBPort	= 0;
	memset(m_strUserName, 0, sizeof(m_strUserName));
	memset(m_strPassword, 0, sizeof(m_strPassword));
	memset(m_strDBName, 0, sizeof(m_strDBName));
	memset(m_strSQL, 0, sizeof(m_strSQL));

	m_uLastError		= 0;

	m_nRobotDataCount	= 0;
	m_pCreatRobotData	= NULL;

	m_uNextPingTime	= 0;
	m_uNextConnectTime	= 0;

	m_bRunning			= false;
	m_bExit				= true;

	m_ProtocolFunc[role_db_create_role]				= &CRoleDBThread::RequestCreateRole;
	m_ProtocolFunc[role_db_save_role]				= &CRoleDBThread::RequestSaveRole;
	m_ProtocolFunc[role_db_role_rename]				= &CRoleDBThread::RequestRename;
	m_ProtocolFunc[role_db_create_mail]				= &CRoleDBThread::RequestCreateMail;
	m_ProtocolFunc[role_db_load_mails]				= &CRoleDBThread::RequestLoadMails;

	m_ProtocolFunc[role_db_update_mail]				= &CRoleDBThread::RequestUpdateMail;
	m_ProtocolFunc[role_db_delete_mail]				= &CRoleDBThread::RequestDeleteMail;
	m_ProtocolFunc[role_db_create_global_mail]		= &CRoleDBThread::RequestCreateGlobalMail;
	m_ProtocolFunc[role_db_delete_expire_mail]		= &CRoleDBThread::RequestDeleteExpireMail;
	m_ProtocolFunc[role_db_create_mail_only]		= &CRoleDBThread::RequestCreateMailOnly;

	m_ProtocolFunc[role_db_update_mail_expire]		= &CRoleDBThread::RequestUpdateMailExpire;
	m_ProtocolFunc[role_db_update_reset_time]		= &CRoleDBThread::RequestUpdateLastResetTime;
	m_ProtocolFunc[role_db_update_pvp_award_time]	= &CRoleDBThread::RequestUpdateLastPVPAwardTime;
	m_ProtocolFunc[role_db_update_td_ranking]		= &CRoleDBThread::RequestUpdateRoleTDRanking;
	m_ProtocolFunc[role_db_update_online_pvp_point]	= &CRoleDBThread::RequestUpdateOnlinePVPPoint;

	m_ProtocolFunc[role_db_online_pvp_award_time]	= &CRoleDBThread::RequestLastOnlinePVPAwardTime;
	m_ProtocolFunc[role_db_update_online_pvp_title]	= &CRoleDBThread::RequestUpdateOnlinePVPTitle;
	m_ProtocolFunc[role_db_add_role_login_record]	= &CRoleDBThread::RequestAddRoleLoginRecord;
	m_ProtocolFunc[role_db_add_gm_log]				= &CRoleDBThread::RecvGMLog;
	m_ProtocolFunc[role_db_query_remain]			= &CRoleDBThread::RecvQueryRemain;

	m_ProtocolFunc[role_db_import_gift_code]		= &CRoleDBThread::RecvAddGiftCode;
	m_ProtocolFunc[role_db_del_gift_code]			= &CRoleDBThread::RecvDelGiftCode;
	m_ProtocolFunc[role_db_update_gift_code]		= &CRoleDBThread::RecvUpdateGiftCode;
	m_ProtocolFunc[role_exchange_gift]				= &CRoleDBThread::RecvExchangeGiftCode;
	m_ProtocolFunc[role_db_query_dnu]				= &CRoleDBThread::RecvQueryDnu;

	m_ProtocolFunc[role_db_query_dau]				= &CRoleDBThread::RecvQueryDau;
	m_ProtocolFunc[role_db_update_perday_reset_time]= &CRoleDBThread::RecvLastPerDayResetTime;
	m_ProtocolFunc[role_db_query_common_remain]		= &CRoleDBThread::RecvQueryCommonRemain;
	m_ProtocolFunc[role_db_create_guild]			= &CRoleDBThread::RecvCreateGuild;
	m_ProtocolFunc[role_db_apply_join_guild]		= &CRoleDBThread::RecvApplyJoinGuild;
	
	m_ProtocolFunc[role_db_refuse_join_guild]		= &CRoleDBThread::RecvRefuseJoinGuild;
	m_ProtocolFunc[role_db_approve_join_guild]		= &CRoleDBThread::RecvApproveJoinGuild;
	m_ProtocolFunc[role_db_player_join_guild]		= &CRoleDBThread::RecvPlayerJoinGuild;
	m_ProtocolFunc[role_db_update_ct_ranking]		= &CRoleDBThread::RecvUpdateCTRanking;
	m_ProtocolFunc[role_db_load_friend_log]			= &CRoleDBThread::RecvLoadFriendLog;

	m_ProtocolFunc[role_db_update_friend]			= &CRoleDBThread::RecvUpdateFriend;
	m_ProtocolFunc[role_db_add_friend_log]			= &CRoleDBThread::RecvAddFriendLog;
	m_ProtocolFunc[role_db_del_friend_log]			= &CRoleDBThread::RecvDelFriendLog;
	m_ProtocolFunc[role_db_create_red_envelope]		= &CRoleDBThread::RecvCreateRedEnvelope;
	m_ProtocolFunc[role_db_update_red_envelope]		= &CRoleDBThread::RecvUpdateRedEnvelope;

	m_ProtocolFunc[role_db_delete_red_envelope]		= &CRoleDBThread::RecvDeleteRedEnvelope;
	m_ProtocolFunc[role_db_add_friend_whisper]		= &CRoleDBThread::RecvAddFriendWhisper;
	m_ProtocolFunc[role_db_request_friend_whisper]	= &CRoleDBThread::RecvRequestFriendWhisper;
}

CRoleDBThread::~CRoleDBThread()
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

	if (m_pRBRoleDBRequest)
	{
		m_pRBRoleDBRequest->Release();
		m_pRBRoleDBRequest	= NULL;
	}

	if (m_pRBRoleDBRespond)
	{
		m_pRBRoleDBRespond->Release();
		m_pRBRoleDBRespond	= NULL;
	}

	SAFE_DELETE_ARR(m_pCreatRobotData);
}

bool CRoleDBThread::Initialize(char *pstrDBIP, char *pstrAccount, char *pstrPassword, char *pstrDBName, unsigned short usDBPort)
{
	strncpy(m_strDBIP, pstrDBIP, sizeof(m_strDBIP));
	m_strDBIP[sizeof(m_strDBIP)-1]	= '\0';
	strncpy(m_strUserName, pstrAccount, sizeof(m_strUserName));
	m_strUserName[sizeof(m_strUserName)-1]	= '\0';
	strncpy(m_strPassword, pstrPassword, sizeof(m_strPassword));
	m_strPassword[sizeof(m_strPassword)-1]	= '\0';
	strncpy(m_strDBName, pstrDBName, sizeof(m_strDBName));
	m_strDBName[sizeof(m_strDBName)-1]	= '\0';
	m_usDBPort	= usDBPort;

	m_pRBRoleDBRequest	= CreateRingBuffer(ROLE_DB_RB_REQUEST_LEN, ROLE_DB_RB_REQUEST_PACK_LEN);
	if (!m_pRBRoleDBRequest)
	{
		g_pFileLog->WriteLog("%s[%d] Create RingBuffer For RoleDB Recv Failed!\n", __FILE__, __LINE__);
		return false;
	}

	m_pRBRoleDBRespond	= CreateRingBuffer(ROLE_DB_RB_RESPOND_LEN, ROLE_DB_RB_RESPOND_PACK_LEN);
	if (!m_pRBRoleDBRespond)
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

	if (!mysql_real_connect(m_pDBHandle, m_strDBIP, m_strUserName, m_strPassword, "", m_usDBPort, NULL, CLIENT_MULTI_STATEMENTS))
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

	if (0 != mysql_select_db(m_pDBHandle, m_strDBName))
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

	if (!CreateProcedure())
	{
		g_pFileLog->WriteLog("%s[%d] Create Tables Procedure!\n", __FILE__, __LINE__);
		return false;
	}

	if (!CreateGameServerList())
	{
		g_pFileLog->WriteLog("%s[%d] Register Game Server Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!LoadAllGSData())
	{
		g_pFileLog->WriteLog("%s[%d] Load All GameServer Data Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!LoadAllActivityData())
	{
		g_pFileLog->WriteLog("%s[%d] Load All ActivityServer Data Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!LoadAllGameMasterData())
	{
		g_pFileLog->WriteLog("%s[%d] Load All Game Master Data Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!LoadAllRoleData())
	{
		g_pFileLog->WriteLog("%s[%d] Load All Role Data Failed!\n", __FILE__, __LINE__);
		return false;
	}

	if (!LoadAllGlobalMail())
	{
		g_pFileLog->WriteLog("%s[%d] Load All Global Mail Failed!\n", __FILE__, __LINE__);
		return false;
	}

	m_bRunning	= true;

	m_RoleDBThread.Create(this, &CRoleDBThread::RoleDBThreadFunc);

	return true;
}

bool CRoleDBThread::SendRoleDBRequest(const void *pPack, const unsigned int uiPackLen)
{
	return m_pRBRoleDBRequest->SndPack(pPack, uiPackLen);
}

const void *CRoleDBThread::GetRoleDBRespond(unsigned int &uiPackLen)
{
	return m_pRBRoleDBRespond->RcvPack(uiPackLen);
}

void CRoleDBThread::RoleDBThreadFunc()
{
	uint64	ullBeginTick		= GetMicroTick();
	uint64	ullNextFrameTick	= 0;
	uint64	ullTickNow			= 0;
	uint64	ullDBThreadFrame	= 0;

	m_bExit	= false;

	while (m_bRunning)
	{
		ullTickNow	= GetMicroTick();

		if (ullTickNow < ullNextFrameTick)
		{
			yield(8);
			continue;
		}

		++ullDBThreadFrame;

		ullNextFrameTick	= ullBeginTick + ullDBThreadFrame * 1000 / DB_SERVER_FRAME;

		DBActive();

		ProcessRequest();
	}

	m_bExit	= true;
}

void CRoleDBThread::SaveAllData()
{
	DBActive();

	ProcessRequest();
}

void CRoleDBThread::Release()
{
	delete this;
}

bool CRoleDBThread::IsExistDB(const char *pstrDBName)
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

bool CRoleDBThread::Query(const char *pstrSQL, const unsigned int uiSQLLen)
{
	if (!m_pDBHandle)
	{
		g_pFileLog->WriteLog("%s[%d] m_pDBHandle Is NULL\n", __FILE__, __LINE__);
		return false;
	}

	if (0 != mysql_real_query(m_pDBHandle, pstrSQL, uiSQLLen))	// 执行单个sql查询
	{
		m_uLastError			= mysql_errno(m_pDBHandle);
		const char	*pstrError	= mysql_error(m_pDBHandle);

		g_pFileLog->WriteLog("%s[%d] mysql_real_query Error:[%u]\n[%s]\n", __FILE__, __LINE__, m_uLastError, pstrError);

		return false;
	}

	return true;
}

void CRoleDBThread::Disconnect()
{
	if (m_pDBHandle)
	{
		mysql_close(m_pDBHandle);
		m_pDBHandle	= NULL;
	}
}

bool CRoleDBThread::CreateDataBase()
{
	if (IsExistDB(m_strDBName))
	{
		// 数据库已存在，直接返回
		return true;
	}

	int	nStrLen	= snprintf(m_strSQL, sizeof(m_strSQL), CREATE_DATABASE_SQL, m_strDBName);
	if (0 != mysql_real_query(m_pDBHandle, m_strSQL, nStrLen))
	{
		m_uLastError			= mysql_errno(m_pDBHandle);
		const char	*pstrError	= mysql_error(m_pDBHandle);

		g_pFileLog->WriteLog("%s[%d] mysql_real_query Error:[%u]\n[%s]\n", __FILE__, __LINE__, m_uLastError, pstrError);

		return false;
	}

	if (0 != mysql_select_db(m_pDBHandle, m_strDBName))
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

	if (!CreateTables())
	{
		g_pFileLog->WriteLog("%s[%d] Create Tables Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!CreateProcedure())
	{
		g_pFileLog->WriteLog("%s[%d] Create Procedure Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!CreateActivityServerList())
	{
		g_pFileLog->WriteLog("%s[%d] Register Activity Server Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!CreateGameMasterList())
	{
		g_pFileLog->WriteLog("%s[%d] Register Game Master Failed\n", __FILE__, __LINE__);
		return false;
	}

	return true;
}

bool CRoleDBThread::CreateGSRobotAccount(const WORD wGameServerID)
{
	for (int nIndex = 0; nIndex < m_nRobotDataCount; ++nIndex)
	{ 
		if (!CreateRobotRole(wGameServerID, m_pCreatRobotData[nIndex]))
		{
			g_pFileLog->WriteLog("%s[%d] Create Role[%s] Failed\n", __FILE__, __LINE__, m_pCreatRobotData[nIndex].strAccount);
			return false;
		}
	}

	return true;
}

bool CRoleDBThread::CreateGSFreeRoleName(const WORD wGameServerID, const UINT uFreeRoleNameCount)
{
	char	strGameServerID[4];
	memset(strGameServerID, 0, sizeof(strGameServerID));

	snprintf(strGameServerID, sizeof(strGameServerID), "%03hu", wGameServerID);

	int	nStrLen = snprintf(m_strSQL, sizeof(m_strSQL), "call CreateFreeName('%s',%hu,%u)", strGameServerID, wGameServerID, uFreeRoleNameCount);
	if (nStrLen <= 0 || nStrLen >= sizeof(m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return false;
	}

	Query(m_strSQL, nStrLen);

	return true;
}

bool CRoleDBThread::CreateTables()
{
	if (!ExecuteSQL(ACCOUNT_TABLE_SQL))
	{
		g_pFileLog->WriteLog("%s[%d] Create Account Table Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!ExecuteSQL(PAYMENT_LIST_TABLE_SQL))
	{
		g_pFileLog->WriteLog("%s[%d] Create Payment List Table Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!ExecuteSQL(ROLE_TABLE_SQL))
	{
		g_pFileLog->WriteLog("%s[%d] Create Role Table Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!ExecuteSQL(FREE_ROLE_NAME_TABLE_SQL))
	{
		g_pFileLog->WriteLog("%s[%d] Create Free Role Name Table Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!ExecuteSQL(ROLE_LOGIN_TABLE_SQL))
	{
		g_pFileLog->WriteLog("%s[%d] Create Role Login Table Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!ExecuteSQL(GUILD_TABLE_SQL))
	{
		g_pFileLog->WriteLog("%s[%d] Create Guild Table Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!ExecuteSQL(APPLY_JOIN_GUILD_TABLE_SQL))
	{
		g_pFileLog->WriteLog("%s[%d] Create Apply Join Guild Table Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!ExecuteSQL(FRIEND_LOG_TABLE_SQL))
	{
		g_pFileLog->WriteLog("%s[%d] Create Friend Log Table Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!ExecuteSQL(FRIEND_RED_ENVELOPE_TABLE_SQL))
	{
		g_pFileLog->WriteLog("%s[%d] Create Friend Red Envelope Table Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!ExecuteSQL(FRIEND_WHISPER_TABLE_SQL))
	{
		g_pFileLog->WriteLog("%s[%d] Create Friend Whisper Table Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!ExecuteSQL(MAIL_TABLE_SQL))
	{
		g_pFileLog->WriteLog("%s[%d] Create Mail Table Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!ExecuteSQL(GLOBAL_MAIL_TABLE_SQL))
	{
		g_pFileLog->WriteLog("%s[%d] Create System Mail Table Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!ExecuteSQL(GAME_SERVER_TABLE_SQL))
	{
		g_pFileLog->WriteLog("%s[%d] Create GameServer Table Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!ExecuteSQL(GLOBAL_DATA_TABLE_SQL))
	{
		g_pFileLog->WriteLog("%s[%d] Create Global Data Table Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!ExecuteSQL(ACTIVITY_SERVER_TABLE_SQL))
	{
		g_pFileLog->WriteLog("%s[%d] Create ActivityServer Table Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!ExecuteSQL(GAME_MASTER_TABLE_SQL))
	{
		g_pFileLog->WriteLog("[%s][%d] Create Game Master Table Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!ExecuteSQL(GM_LOG_TABLE_SQL))
	{
		g_pFileLog->WriteLog("[%s][%d] Create GMLog Table Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!ExecuteSQL(GIFT_CODE_TABLE_SQL))
	{
		g_pFileLog->WriteLog("[%s][%d] Create Gift Code Table Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!ExecuteSQL(EXCHANGE_GIFT_CODE_TABLE_SQL))
	{
		g_pFileLog->WriteLog("[%s][%d] Create Exchange Gift Code Table Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!ExecuteSQL(CLIMB_TOWER_TABLE_SQL))
	{
		g_pFileLog->WriteLog("[%s][%d] Create Climb Tower Table Failed\n", __FILE__, __LINE__);
		return false;
	}

	return true;
}

bool CRoleDBThread::CreateProcedure()
{
	if (!ExecuteSQL(DROP_PROCEDURE_CREATE_ROLE_SQL))
	{
		g_pFileLog->WriteLog("[%s][%d] Drop Procedure[Create Role] Failed\n", __FILE__, __LINE__);
		return false;
	}

	ExecuteSQL(DROP_PROCEDURE_CREATE_FREE_NAME);

	if (!ExecuteSQL(DROP_PROCEDURE_CREATE_MAIL))
	{
		g_pFileLog->WriteLog("[%s][%d] Drop Procedure[Create Mail] Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!ExecuteSQL(DROP_PROCEDURE_EXCHANGE_GIFT_CODE))
	{
		g_pFileLog->WriteLog("[%s][%d] Drop Procedure[Exchange Gift Code] Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!ExecuteSQL(DROP_PROCEDURE_QUERY_COMMON_REMAIN))
	{
		g_pFileLog->WriteLog("[%s][%d] Drop Procedure[Query Common Remain] Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!ExecuteSQL(DROP_PROCEDURE_ROLE_RENAME))
	{
		g_pFileLog->WriteLog("[%s][%d] Drop Procedure[Role Rename] Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!ExecuteSQL(DROP_PROCEDURE_CREATE_GUILD))
	{
		g_pFileLog->WriteLog("[%s][%d] Drop Procedure[Create Guild] Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!ExecuteSQL(DROP_PROCEDURE_APPLY_JOIN_GUILD))
	{
		g_pFileLog->WriteLog("[%s][%d] Drop Procedure[Apply Join Guild] Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!ExecuteSQL(DROP_PROCEDURE_REFUSE_JOIN_GUILD))
	{
		g_pFileLog->WriteLog("[%s][%d] Drop Procedure[Refuse Join Guild] Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!ExecuteSQL(DROP_PROCEDURE_APPROVE_JOIN_GUILD))
	{
		g_pFileLog->WriteLog("[%s][%d] Drop Procedure[Approve Join Guild] Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!ExecuteSQL(DROP_PROCEDURE_PLAYER_JOIN_GUILD))
	{
		g_pFileLog->WriteLog("[%s][%d] Drop Procedure[Player Join Guild] Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!ExecuteSQL(DROP_PROCEDURE_PLAYER_CLIMB_TOWER))
	{
		g_pFileLog->WriteLog("[%s][%d] Drop Procedure[Player Climb Tower] Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!ExecuteSQL(DROP_PROCEDURE_LOAD_FRIEND_LOG))
	{
		g_pFileLog->WriteLog("[%s][%d] Drop Procedure[Load Friend Log] Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!ExecuteSQL(DROP_PROCEDURE_UPDATE_FRIEND_DATA))
	{
		g_pFileLog->WriteLog("[%s][%d] Drop Procedure[Update Friend Data] Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!ExecuteSQL(DROP_PROCEDURE_ADD_FRIEND_LOG))
	{
		g_pFileLog->WriteLog("[%s][%d] Drop Procedure[Add Friend Log] Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!ExecuteSQL(DROP_PROCEDURE_DELETE_FRIEND_LOG))
	{
		g_pFileLog->WriteLog("[%s][%d] Drop Procedure[Delete Friend Log] Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!ExecuteSQL(DROP_PROCEDURE_CREATE_RED_ENVELOPE))
	{
		g_pFileLog->WriteLog("[%s][%d] Drop Procedure[Create Red Envelope] Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!ExecuteSQL(DROP_PROCEDURE_UPDATE_RED_ENVELOPE))
	{
		g_pFileLog->WriteLog("[%s][%d] Drop Procedure[Update Red Envelope] Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!ExecuteSQL(DROP_PROCEDURE_DELETE_RED_ENVELOPE))
	{
		g_pFileLog->WriteLog("[%s][%d] Drop Procedure[Delete Red Envelope] Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!ExecuteSQL(DROP_PROCEDURE_ADD_FRIEND_WHISPER))
	{
		g_pFileLog->WriteLog("[%s][%d] Drop Procedure[Add Friend Whisper] Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!ExecuteSQL(DROP_PROCEDURE_LOAD_FRIEND_WHISPER))
	{
		g_pFileLog->WriteLog("[%s][%d] Drop Procedure[Load Friend Whisper] Failed\n", __FILE__, __LINE__);
		return false;
	}

	ExecuteSQL(PROCEDURE_CREATE_FREE_NAME);

	if (!ExecuteSQL(PROCEDURE_CREATE_ROLE_SQL))
	{
		g_pFileLog->WriteLog("[%s][%d] Create Procedure[Create Role] Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!ExecuteSQL(PROCEDURE_CREATE_MAIL))
	{
		g_pFileLog->WriteLog("[%s][%d] Create Procedure[Create Mail] Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!ExecuteSQL(PROCEDURE_EXCHANGE_GIFT_CODE))
	{
		g_pFileLog->WriteLog("[%s][%d] Create Procedure[Exchange Gife Code] Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!ExecuteSQL(PROCEDURE_QUERY_COMMON_REMAIN))
	{
		g_pFileLog->WriteLog("[%s][%d] Create Procedure[Query Common Remain] Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!ExecuteSQL(PROCEDURE_ROLE_RENAME))
	{
		g_pFileLog->WriteLog("[%s][%d] Create Procedure[Role Rename] Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!ExecuteSQL(PROCEDURE_CREATE_GUILD))
	{
		g_pFileLog->WriteLog("[%s][%d] Create Procedure[Create Guild] Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!ExecuteSQL(PROCEDURE_APPLY_JOIN_GUILD))
	{
		g_pFileLog->WriteLog("[%s][%d] Create Procedure[Apply Join Guild] Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!ExecuteSQL(PROCEDURE_REFUSE_JOIN_GUILD))
	{
		g_pFileLog->WriteLog("[%s][%d] Create Procedure[Refuse Join Guild] Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!ExecuteSQL(PROCEDURE_APPROVE_JOIN_GUILD))
	{
		g_pFileLog->WriteLog("[%s][%d] Create Procedure[Approve Join Guild] Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!ExecuteSQL(PROCEDURE_PLAYER_JOIN_GUILD))
	{
		g_pFileLog->WriteLog("[%s][%d] Create Procedure[Player Join Guild] Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!ExecuteSQL(PROCEDURE_PLAYER_CLIMB_TOWER))
	{
		g_pFileLog->WriteLog("[%s][%d] Create Procedure[Player Climb Tower] Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!ExecuteSQL(PROCEDURE_LOAD_FRIEND_LOG))
	{
		g_pFileLog->WriteLog("[%s][%d] Create Procedure[Load Friend Log] Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!ExecuteSQL(PROCEDURE_UPDATE_FRIEND_DATA))
	{
		g_pFileLog->WriteLog("[%s][%d] Create Procedure[Update Friend Data] Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!ExecuteSQL(PROCEDURE_ADD_FRIEND_LOG))
	{
		g_pFileLog->WriteLog("[%s][%d] Create Procedure[Add Friend Log] Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!ExecuteSQL(PROCEDURE_DELETE_FRIEND_LOG))
	{
		g_pFileLog->WriteLog("[%s][%d] Create Procedure[Delete Friend Log] Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!ExecuteSQL(PROCEDURE_CREATE_RED_ENVELOPE))
	{
		g_pFileLog->WriteLog("[%s][%d] Create Procedure[Create Red Envelope] Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!ExecuteSQL(PROCEDURE_UPDATE_RED_ENVELOPE))
	{
		g_pFileLog->WriteLog("[%s][%d] Create Procedure[Update Red Envelope] Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!ExecuteSQL(PROCEDURE_DELETE_RED_ENVELOPE))
	{
		g_pFileLog->WriteLog("[%s][%d] Create Procedure[Delete Red Envelope] Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!ExecuteSQL(PROCEDURE_ADD_FRIEND_WHISPER))
	{
		g_pFileLog->WriteLog("[%s][%d] Create Procedure[Add Friend Whisper] Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!ExecuteSQL(PROCEDURE_LOAD_FRIEND_WHISPER))
	{
		g_pFileLog->WriteLog("[%s][%d] Create Procedure[Load Friend Whisper] Failed\n", __FILE__, __LINE__);
		return false;
	}

	return true;
}

bool CRoleDBThread::CreateGameServerList()
{
	IIniFile	*m_pIniFile	= OpenIniFile("Config.ini");
	if (NULL == m_pIniFile)
	{
		g_pFileLog->WriteLog("%s[%d] Open Config.ini Failed\n", __FILE__, __LINE__);
		return false;
	}

	int		nGSCount;
	int		nGSID;
	char	strKey[32];
	char	strGSName[MAX_SERVER_NAME_LEN];
	char	strGSIP[MAX_IP_LEN];
	int		nPlayerCount;
	int		nFreeRoleNameCount;

	m_pIniFile->GetInteger("CenterServer", "GSCount", 0, &nGSCount);

	for (int nIndex = 0; nIndex < nGSCount; ++nIndex)
	{
		snprintf(strKey, sizeof(strKey), "GS%d_ID", nIndex+1);
		if (!m_pIniFile->GetInteger("CenterServer", strKey, 0, &nGSID))
		{
			g_pFileLog->WriteLog("%s[%d] Read GS[%d] Name Failed\n", __FILE__, __LINE__, nIndex+1);
			m_pIniFile->Release();
			m_pIniFile	= NULL;
			return false;
		}

		snprintf(strKey, sizeof(strKey), "GS%d_Name", nIndex+1);
		if (!m_pIniFile->GetString("CenterServer", strKey, "", strGSName, sizeof(strGSName)))
		{
			g_pFileLog->WriteLog("%s[%d] Read GS[%d] Name Failed\n", __FILE__, __LINE__, nIndex+1);
			m_pIniFile->Release();
			m_pIniFile	= NULL;
			return false;
		}
		strGSName[sizeof(strGSName)-1]	= '\0';

		snprintf(strKey, sizeof(strKey), "GS%d_IP", nIndex+1);
		if (!m_pIniFile->GetString("CenterServer", strKey, "", strGSIP, sizeof(strGSIP)))
		{
			g_pFileLog->WriteLog("%s[%d] Read GS[%d] IP Failed\n", __FILE__, __LINE__, nIndex+1);
			m_pIniFile->Release();
			m_pIniFile	= NULL;
			return false;
		}
		strGSIP[sizeof(strGSIP)-1]	= '\0';

		snprintf(strKey, sizeof(strKey), "GS%d_PlayerCount", nIndex+1);
		if (!m_pIniFile->GetInteger("CenterServer", strKey, 0, &nPlayerCount))
		{
			g_pFileLog->WriteLog("%s[%d] Read GS[%d] PlayerCount Failed\n", __FILE__, __LINE__, nIndex+1);
			m_pIniFile->Release();
			m_pIniFile	= NULL;
			return false;
		}

		snprintf(strKey, sizeof(strKey), "GS%d_RoleNameCount", nIndex+1);
		if (!m_pIniFile->GetInteger("CenterServer", strKey, 0, &nFreeRoleNameCount))
		{
			g_pFileLog->WriteLog("%s[%d] Read GS[%d] PlayerCount Failed\n", __FILE__, __LINE__, nIndex+1);
			m_pIniFile->Release();
			m_pIniFile	= NULL;
			return false;
		}

		CreateGameServer(nGSID, strGSName, strGSIP, nPlayerCount, nFreeRoleNameCount);
	}

	m_pIniFile->Release();
	m_pIniFile	= NULL;

	return true;
}

bool CRoleDBThread::CreateGameServer(const WORD wGSID, char (&strGSName)[MAX_SERVER_NAME_LEN], char (&strGSIP)[MAX_IP_LEN], const WORD wPlayerCount, const UINT uFreeRoleNameCount)
{
	if (!RegistGameServer(wGSID, strGSName, strGSIP, wPlayerCount))
	{
		//g_pFileLog->WriteLog("%s[%d] Create GS[%hu] GameServer Table Failed\n", __FILE__, __LINE__, wGSID);
		return false;
	}

	if (!CreateGSRobotAccount(wGSID))
	{
		g_pFileLog->WriteLog("%s[%d] Create Robot Data Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (!CreateGSFreeRoleName(wGSID, uFreeRoleNameCount))
	{
		g_pFileLog->WriteLog("%s[%d] Create Free Role Name Failed\n", __FILE__, __LINE__);
		return false;
	}

	return true;
}

bool CRoleDBThread::RegistGameServer(const WORD wGSID, char (&strGSName)[MAX_SERVER_NAME_LEN], char (&strGSIP)[MAX_IP_LEN], const WORD wPlayerCount)
{
	char			*pszPos		= m_strSQL;
	unsigned int	uIP			= inet_addr(strGSIP);
	int				nStrLen		= 0;
	unsigned int	uLeftSize	= sizeof(m_strSQL);

	nStrLen = snprintf(pszPos, uLeftSize, "insert into gameserver (ID,Name,IP,PlayerCount) values(%hu,'", wGSID);
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return false;
	}

	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	int	nLength	= strlen(strGSName);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return false;
		}
		nStrLen = mysql_real_escape_string(m_pDBHandle, pszPos, strGSName, nLength);
		if (nStrLen <= 0 || nStrLen > uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_real_escape_string SQL Error\n", __FILE__, __LINE__);
			return false;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen = snprintf(pszPos, uLeftSize, "',%u,%hu)", uIP, wPlayerCount);
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return false;
	}

	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	if (!Query(m_strSQL, pszPos-m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return false;
	}

	if (1 != mysql_affected_rows(m_pDBHandle))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return false;
	}

	return true;
}

bool CRoleDBThread::CreateActivityServerList()
{
	IIniFile	*pIniFile	= OpenIniFile("Config.ini");
	if (NULL == pIniFile)
	{
		g_pFileLog->WriteLog("%s[%d] Open Config.ini Failed\n", __FILE__, __LINE__);
		return false;
	}

	int		nASCount;
	int		nASID;
	char	strKey[32];
	char	strASName[MAX_SERVER_NAME_LEN];
	char	strASIP[MAX_IP_LEN];

	pIniFile->GetInteger("CenterServer", "ASCount", 0, &nASCount);

	for (int nIndex = 0; nIndex < nASCount; ++nIndex)
	{
		snprintf(strKey, sizeof(strKey), "AS%d_ID", nIndex+1);
		if (!pIniFile->GetInteger("CenterServer", strKey, 0, &nASID))
		{
			g_pFileLog->WriteLog("%s[%d] Read AS[%d] Name Failed\n", __FILE__, __LINE__, nIndex+1);
			pIniFile->Release();
			pIniFile	= NULL;
			return false;
		}

		snprintf(strKey, sizeof(strKey), "AS%d_Name", nIndex+1);
		if (!pIniFile->GetString("CenterServer", strKey, "", strASName, sizeof(strASName)))
		{
			g_pFileLog->WriteLog("%s[%d] Read AS[%d] Name Failed\n", __FILE__, __LINE__, nIndex+1);
			pIniFile->Release();
			pIniFile	= NULL;
			return false;
		}
		strASName[sizeof(strASName)-1]	= '\0';

		snprintf(strKey, sizeof(strKey), "AS%d_IP", nIndex+1);
		if (!pIniFile->GetString("CenterServer", strKey, "", strASIP, sizeof(strASIP)))
		{
			g_pFileLog->WriteLog("%s[%d] Read AS[%d] IP Failed\n", __FILE__, __LINE__, nIndex+1);
			pIniFile->Release();
			pIniFile	= NULL;
			return false;
		}
		strASIP[sizeof(strASIP)-1]	= '\0';

		if (!CreateActivityServer(nASID, strASName, strASIP, false))
		{
			g_pFileLog->WriteLog("%s[%d] Create Procedure[Create CreateActivityServer] Failed!\n", __FILE__, __LINE__);
			return false;
		}
	}

	pIniFile->Release();
	pIniFile	= NULL;

	return true;
}

bool CRoleDBThread::CreateGameMasterList()
{
	IIniFile	*pIniFile	= OpenIniFile("Config.ini");
	if (NULL == pIniFile)
	{
		g_pFileLog->WriteLog("[%s][%d] Open Config.ini Failed\n", __FILE__, __LINE__);
		return false;
	}

	int		nGMCount;
	char	strKey[32];
	char	strAccount[MAX_ACCOUNT_LEN];
	char	strPassword[MAX_PASSWORD_LEN];

	pIniFile->GetInteger("CenterServer", "GMCount", 0, &nGMCount);

	for (int nIndex = 0; nIndex < nGMCount; ++nIndex)
	{
		snprintf(strKey, sizeof(strKey), "GM%d_Account", nIndex+1);
		if (!pIniFile->GetString("CenterServer", strKey, "", strAccount, sizeof(strAccount)))
		{
			g_pFileLog->WriteLog("%s[%d] Read GM[%d] Account Failed\n", __FILE__, __LINE__, nIndex+1);
			pIniFile->Release();
			pIniFile	= NULL;
			return false;
		}
		strAccount[sizeof(strAccount)-1]	= '\0';

		snprintf(strKey, sizeof(strKey), "GM%d_Password", nIndex+1);
		if (!pIniFile->GetString("CenterServer", strKey, "", strPassword, sizeof(strPassword)))
		{
			g_pFileLog->WriteLog("%s[%d] Read GM[%d] Password Failed\n", __FILE__, __LINE__, nIndex+1);
			pIniFile->Release();
			pIniFile	= NULL;
			return false;
		}
		strPassword[sizeof(strPassword)-1]	= '\0';

		if (!CreateGameMaster(strAccount, strPassword, false))
		{
			g_pFileLog->WriteLog("%s[%d] Create Procedure[Create CreateGameMaster] Failed!\n", __FILE__, __LINE__);
			return false;
		}
	}

	pIniFile->Release();
	pIniFile	= NULL;

	return true;
}

bool CRoleDBThread::CreateActivityServer(const WORD wASID, char (&strASName)[MAX_SERVER_NAME_LEN], char (&strASIP)[MAX_IP_LEN], const bool bLoadAS)
{
	if (!RegistActivityServer(wASID, strASName, strASIP))
	{
		g_pFileLog->WriteLog("%s[%d] Create AS[%hu] ActivityServer Table Failed\n", __FILE__, __LINE__, wASID);
		return false;
	}

	if (bLoadAS)
	{
		if (!LoadActivityData(wASID))
		{
			g_pFileLog->WriteLog("%s[%d] Load AS[%hu] Data Error\n", __FILE__, __LINE__, wASID);
			return false;
		}
	}

	return true;
}

bool CRoleDBThread::CreateGameMaster(char (&strAccount)[MAX_ACCOUNT_LEN], char (&strPassword)[MAX_PASSWORD_LEN], const bool bLoadGM)
{
	if (!RegistGameMaster(strAccount, strPassword))
	{
		g_pFileLog->WriteLog("%s[%d] Create GameMaster Account[%s] Failed\n", __FILE__, __LINE__, strAccount);
		return false;
	}

	if (bLoadGM)
	{
		if (!LoadGameMasterData(strAccount))
		{
			g_pFileLog->WriteLog("%s[%d] Load GameMaster[%s] Data Error\n", __FILE__, __LINE__, strAccount);
			return false;
		}
	}

	return true;
}

bool CRoleDBThread::RegistActivityServer(const WORD wASID, char (&strASName)[MAX_SERVER_NAME_LEN], char (&strASIP)[MAX_IP_LEN])
{
	char			*pszPos		= m_strSQL;
	unsigned int	uIP			= inet_addr(strASIP);
	int				nStrLen		= 0;
	unsigned int	uLeftSize	= sizeof(m_strSQL);

	nStrLen = snprintf(pszPos, uLeftSize, "insert into activityserver (ID,Name,IP) values(%hu,'", wASID);
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return false;
	}

	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	int	nLength	= strlen(strASName);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return false;
		}
		nStrLen = mysql_real_escape_string(m_pDBHandle, pszPos, strASName, nLength);
		if (nStrLen <= 0 || nStrLen > uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_real_escape_string SQL Error\n", __FILE__, __LINE__);
			return false;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen = snprintf(pszPos, uLeftSize, "',%u)", uIP);
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return false;
	}

	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	if (!Query(m_strSQL, pszPos-m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return false;
	}

	if (1 != mysql_affected_rows(m_pDBHandle))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return false;
	}

	return true;
}

bool CRoleDBThread::RegistGameMaster(char (&strAccount)[MAX_ACCOUNT_LEN], char (&strPassword)[MAX_PASSWORD_LEN])
{
	char			*pszPos		= m_strSQL;
	int				nStrLen		= 0;
	unsigned int	uLeftSize	= sizeof(m_strSQL);
	int				nLength		= 0;

	nStrLen = snprintf(pszPos, uLeftSize, "insert into gamemaster (Account,Password) values('");
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return false;
	}

	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= strlen(strAccount);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return false;
		}
		nStrLen = mysql_real_escape_string(m_pDBHandle, pszPos, strAccount, nLength);
		if (nStrLen <= 0 || nStrLen > uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_real_escape_string SQL Error\n", __FILE__, __LINE__);
			return false;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen = snprintf(pszPos, uLeftSize, "','");
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return false;
	}

	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= strlen(strPassword);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return false;
		}
		nStrLen = mysql_real_escape_string(m_pDBHandle, pszPos, strPassword, nLength);
		if (nStrLen <= 0 || nStrLen > uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_real_escape_string SQL Error\n", __FILE__, __LINE__);
			return false;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen = snprintf(pszPos, uLeftSize, "')");
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return false;
	}

	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	if (!Query(m_strSQL, pszPos-m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return false;
	}

	if (1 != mysql_affected_rows(m_pDBHandle))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return false;
	}

	return true;
}

bool CRoleDBThread::ExecuteSQL(const char *pstrSQL)
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

bool CRoleDBThread::LoadAllRoleData()
{
	int	nStrLen	= snprintf(m_strSQL, sizeof(m_strSQL), PROCEDURE_LOAD_ALL_ROLE_SQL);

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

	m_pQueryRes = mysql_use_result(m_pDBHandle);
	if (!m_pQueryRes)
	{
		g_pFileLog->WriteLog("%s[%d] mysql_use_result Failed!\n", __FILE__, __LINE__);
		return false;
	}

	int nColIndex;

	while (NULL != (m_pRow = mysql_fetch_row(m_pQueryRes)))
	{
		unsigned long	*pRowLength	= mysql_fetch_lengths(m_pQueryRes);
		if (!pRowLength)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_fetch_lengths Failed\n", __FILE__, __LINE__);
			return false;
		}

		SDBRoleInfo	tagRoleInfo;
		memset(&tagRoleInfo, 0, sizeof(tagRoleInfo));

		nColIndex	= 0;
		tagRoleInfo.uiPlayerID	= (UINT)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		tagRoleInfo.wGameServerID	= (WORD)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		strncpy(tagRoleInfo.strAccount, m_pRow[nColIndex], sizeof(tagRoleInfo.strAccount));
		tagRoleInfo.strAccount[sizeof(tagRoleInfo.strAccount)-1]	= '\0';

		++nColIndex;
		strncpy(tagRoleInfo.strRoleName, m_pRow[nColIndex], sizeof(tagRoleInfo.strRoleName));
		tagRoleInfo.strRoleName[sizeof(tagRoleInfo.strRoleName)-1]	= '\0';

		++nColIndex;// CreateTime
		tagRoleInfo.nCreateTime	= (int)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;// OnlineTime
		tagRoleInfo.nOnlineTime	= (int)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;// OfflineTime
		tagRoleInfo.nOfflineTime	= (int)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		tagRoleInfo.uLoginTimes	= (UINT)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		tagRoleInfo.byRobot			= (BYTE)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		tagRoleInfo.byGameMaster	= (BYTE)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		tagRoleInfo.wGuildID	= (WORD)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		tagRoleInfo.byGuildJob	= (BYTE)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		tagRoleInfo.nPower	= (int)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		tagRoleInfo.byImageID	= (BYTE)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		tagRoleInfo.byHeroCount	= (BYTE)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		if (m_pRow[nColIndex])
		{
			memcpy(tagRoleInfo.tagHeroData, m_pRow[nColIndex], sizeof(tagRoleInfo.tagHeroData));
		}

		++nColIndex;
		tagRoleInfo.byLevel	= (BYTE)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		tagRoleInfo.nHeroExpPool = (int)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		tagRoleInfo.nDiamond = (int)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		tagRoleInfo.uRaffleTicket = (UINT)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		tagRoleInfo.uTDMoney = (UINT)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		tagRoleInfo.nGold = (int)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		tagRoleInfo.nPvPCoin = (int)strtoul(m_pRow[nColIndex], NULL, 10);
		
		++nColIndex;
		tagRoleInfo.nExpeditionCoin = (int)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		tagRoleInfo.nLingShi = (int)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		tagRoleInfo.nGodSoulsCoin = (int)strtoul(m_pRow[nColIndex], NULL, 10);
			
		++nColIndex;
		tagRoleInfo.nEnergy = (int)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		tagRoleInfo.nExp = (int)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		tagRoleInfo.byItemCount = (BYTE)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		if (m_pRow[nColIndex])
		{
			memcpy(tagRoleInfo.tagItemData, m_pRow[nColIndex], sizeof(tagRoleInfo.tagItemData));
		}

		++nColIndex;
		tagRoleInfo.byResetTimes = (BYTE)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		if (m_pRow[nColIndex])
		{
			memcpy(tagRoleInfo.tagMapData, m_pRow[nColIndex], sizeof(tagRoleInfo.tagMapData));
		}

		++nColIndex;
		if (m_pRow[nColIndex])
		{
			memcpy(tagRoleInfo.tagMissionData, m_pRow[nColIndex], sizeof(tagRoleInfo.tagMissionData));
		}

		++nColIndex;
		tagRoleInfo.nLotteryScore = (int)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		tagRoleInfo.wSkillPoint = (WORD)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		tagRoleInfo.nRecvoerSkillPointTime = (int)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		tagRoleInfo.byBuySkillPointTimes = (int)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		if (m_pRow[nColIndex])
		{
			memcpy(tagRoleInfo.tagEmbattleInfo, m_pRow[nColIndex], sizeof(tagRoleInfo.tagEmbattleInfo));
		}

		++nColIndex;
		tagRoleInfo.byBuyEnergyTimes = (BYTE)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		tagRoleInfo.byBuyGoldTimes = (BYTE)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		tagRoleInfo.byVipLevel = (BYTE)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		if (m_pRow[nColIndex])
		{
			memcpy(tagRoleInfo.byVipGiftSaleState, m_pRow[nColIndex], sizeof(tagRoleInfo.byVipGiftSaleState));
		}

		++nColIndex;
		tagRoleInfo.uRechargeMoney			= (int)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		tagRoleInfo.uiTotalRechargeDiamond	= (int)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		tagRoleInfo.uTotalRechargeTimes		= (int)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		tagRoleInfo.byFirstRechargeAward	= (BYTE)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		tagRoleInfo.nLastResetTime			= (int)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		tagRoleInfo.nLastPerDayResetTime	= (int)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		tagRoleInfo.uWeekCardDays			= (UINT)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		tagRoleInfo.uMonthCardDays			= (UINT)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		tagRoleInfo.uPVPDefendPower			= (UINT)strtoul(m_pRow[nColIndex]	, NULL, 10);

		++nColIndex;
		tagRoleInfo.wPVPWinTimes			= (WORD)strtoul(m_pRow[nColIndex]	, NULL, 10);

		++nColIndex;
		tagRoleInfo.wRanking				= (WORD)strtoul(m_pRow[nColIndex]	, NULL, 10);

		++nColIndex;
		tagRoleInfo.wLastRanking			= (WORD)strtoul(m_pRow[nColIndex]	, NULL, 10);

		++nColIndex;
		tagRoleInfo.nNextPVPTime			= (int)strtoul(m_pRow[nColIndex]	, NULL, 10);

		++nColIndex;
		tagRoleInfo.byChallengeTimes		= (BYTE)strtoul(m_pRow[nColIndex]	, NULL, 10);

		++nColIndex;
		tagRoleInfo.byPVPDefendHeroCount	= (BYTE)strtoul(m_pRow[nColIndex]	, NULL, 10);

		++nColIndex;
		if (m_pRow[nColIndex])
		{
			memcpy(tagRoleInfo.tagDefendLineup, m_pRow[nColIndex], sizeof(tagRoleInfo.tagDefendLineup));
		}

		++nColIndex;
		if (m_pRow[nColIndex])
		{
			memcpy(&tagRoleInfo.tagPvpDefendAddInfo, m_pRow[nColIndex], sizeof(tagRoleInfo.tagPvpDefendAddInfo));
		}

		++nColIndex;
		tagRoleInfo.byPVPAttackHeroCount	= (BYTE)strtoul(m_pRow[nColIndex]	, NULL, 10);

		++nColIndex;
		if (m_pRow[nColIndex])
		{
			memcpy(tagRoleInfo.tagAttackLineup, m_pRow[nColIndex], sizeof(tagRoleInfo.tagAttackLineup));
		}

		++nColIndex;
		if (m_pRow[nColIndex])
		{
			memcpy(&tagRoleInfo.tagPvpAttackAddInfo, m_pRow[nColIndex], sizeof(tagRoleInfo.tagPvpAttackAddInfo));
		}

		++nColIndex;
		if (m_pRow[nColIndex])
		{
			memcpy(tagRoleInfo.tagAchievementData, m_pRow[nColIndex], sizeof(tagRoleInfo.tagAchievementData));
		}

		++nColIndex;
		tagRoleInfo.wAchieveValue    = (UINT)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		tagRoleInfo.byAchieveAwardCount = (UINT)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		tagRoleInfo.uiGoldLotteryTime    = (UINT)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		tagRoleInfo.uiDiamondLotteryTime = (UINT)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		tagRoleInfo.byGoldLotteryTimes = (BYTE)strtoul(m_pRow[nColIndex], NULL, 10);
		
		++nColIndex;
		tagRoleInfo.uiDiamondLotteryTenTimes = (UINT)strtoul(m_pRow[nColIndex], NULL, 10);
		
		++nColIndex;
		if (m_pRow[nColIndex])
		{
			memcpy(tagRoleInfo.tagDailyTask, m_pRow[nColIndex], sizeof(tagRoleInfo.tagDailyTask));
		}

		++nColIndex;
		tagRoleInfo.uiDailyTastNextResetTime = (UINT)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		tagRoleInfo.wDailyActive = (UINT)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		tagRoleInfo.byDailyAwardCount = (UINT)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		tagRoleInfo.byInductStep = (BYTE)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		if (m_pRow[nColIndex])
		{
			memcpy(tagRoleInfo.byInductTask, m_pRow[nColIndex], sizeof(tagRoleInfo.byInductTask));
		}

		++nColIndex;
		tagRoleInfo.byOwnMagicNum = (BYTE)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		if (m_pRow[nColIndex])
		{
			memcpy(tagRoleInfo.tagMagicData, m_pRow[nColIndex], sizeof(tagRoleInfo.tagMagicData));
		}

		++nColIndex;
		if (m_pRow[nColIndex])
		{
			memcpy(tagRoleInfo.tagStoreInfo, m_pRow[nColIndex], sizeof(tagRoleInfo.tagStoreInfo));
		}

		++nColIndex;
		if (m_pRow[nColIndex])
		{
			memcpy(&tagRoleInfo.tagExpedition, m_pRow[nColIndex], sizeof(tagRoleInfo.tagExpedition));
		}

		++nColIndex;
		if (m_pRow[nColIndex])
		{
			memcpy(&tagRoleInfo.tagFriend, m_pRow[nColIndex], sizeof(tagRoleInfo.tagFriend));
		}

		++nColIndex;
		tagRoleInfo.byPVPRecordCount	= pRowLength[nColIndex] / sizeof(SDBPVPRecord);
		if (m_pRow[nColIndex])
		{
			memcpy(&tagRoleInfo.tagPVPRecord, m_pRow[nColIndex], sizeof(tagRoleInfo.tagPVPRecord));
		}

		++nColIndex;
		if (m_pRow[nColIndex])
		{
			memcpy(&tagRoleInfo.tagHeroPalace, m_pRow[nColIndex], sizeof(tagRoleInfo.tagHeroPalace));
		}

		++nColIndex;
		if (m_pRow[nColIndex])
		{
			memcpy(&tagRoleInfo.tagCave, m_pRow[nColIndex], sizeof(tagRoleInfo.tagCave));
		}

		++nColIndex;
		if (m_pRow[nColIndex])
		{
			memcpy(&tagRoleInfo.bySignInTab, m_pRow[nColIndex], sizeof(tagRoleInfo.bySignInTab));
		}

		++nColIndex;
		if (m_pRow[nColIndex])
		{
			memcpy(&tagRoleInfo.byFeatureState, m_pRow[nColIndex], sizeof(tagRoleInfo.byFeatureState));
		}

		++nColIndex;
		if (m_pRow[nColIndex])
		{
			memcpy(&tagRoleInfo.tagMagicMatrix, m_pRow[nColIndex], sizeof(tagRoleInfo.tagMagicMatrix));
		}

		++nColIndex;
		if (m_pRow[nColIndex])
		{
			memcpy(&tagRoleInfo.tagTowerDefence, m_pRow[nColIndex], sizeof(tagRoleInfo.tagTowerDefence));
		}

		++nColIndex;
		if (m_pRow[nColIndex])
		{
			memcpy(&tagRoleInfo.tagTrailTomb, m_pRow[nColIndex], sizeof(tagRoleInfo.tagTrailTomb));
		}

		++nColIndex;
		if (m_pRow[nColIndex])
		{
			memcpy(&tagRoleInfo.tagClimbingTower, m_pRow[nColIndex], sizeof(tagRoleInfo.tagClimbingTower));
		}

		++nColIndex;
		tagRoleInfo.uCTRanking = (UINT)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		tagRoleInfo.uOnlinePVPPoint = (UINT)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		tagRoleInfo.uOnlinePVPLastRanking = (UINT)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		tagRoleInfo.uOnlinePVPFightCount = (UINT)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		tagRoleInfo.uOnlinePVPWinCount = (UINT)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		tagRoleInfo.byOnlinePVPLastTitle = (BYTE)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		if (m_pRow[nColIndex])
		{
			memcpy(&tagRoleInfo.tagOnlinePVP, m_pRow[nColIndex], sizeof(tagRoleInfo.tagOnlinePVP));
		}

		++nColIndex;
		if (m_pRow[nColIndex])
		{
			memcpy(&tagRoleInfo.tagCustomData, m_pRow[nColIndex], sizeof(tagRoleInfo.tagCustomData));
		}

		++nColIndex;
		if (m_pRow[nColIndex])
		{
			memcpy(&tagRoleInfo.uRechargeTimes, m_pRow[nColIndex], sizeof(tagRoleInfo.uRechargeTimes));
		}

		++nColIndex;
		tagRoleInfo.wMainLineId = (WORD)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		tagRoleInfo.byCurMainLineDoTimes = (BYTE)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		if (m_pRow[nColIndex])
		{
			memcpy(&tagRoleInfo.byMainLineState, m_pRow[nColIndex], sizeof(tagRoleInfo.byMainLineState));
		}

		g_pICenterServerLogic->AddRole(&tagRoleInfo);
	}

	mysql_free_result(m_pQueryRes);

	return true;
}

bool CRoleDBThread::LoadAllGlobalMail()
{
	int	nStrLen	= snprintf(m_strSQL, sizeof(m_strSQL), PROCEDURE_LOAD_ALL_GLOBAL_MAIL);

	if (nStrLen <= 0 || nStrLen >= sizeof(m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] nStrLen Is Error:[%d]\n", __FILE__, __LINE__, nStrLen);
		return false;
	}

	if (!Query(m_strSQL, nStrLen))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return false;
	}

	m_pQueryRes = mysql_use_result(m_pDBHandle);
	if (!m_pQueryRes)
	{
		g_pFileLog->WriteLog("%s[%d] mysql_use_result Failed\n", __FILE__, __LINE__);
		return false;
	}

	int nColIndex;
	int	nDataLen;

	while (NULL != (m_pRow = mysql_fetch_row(m_pQueryRes)))
	{
		unsigned long	*pRowLength	= mysql_fetch_lengths(m_pQueryRes);
		if (!pRowLength)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_fetch_lengths Failed\n", __FILE__, __LINE__);
			continue;
		}

		SGlobalMailInfo	tagMailInfo	= {0};

		nColIndex	= 0;
		tagMailInfo.uiMailID	= (UINT)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		tagMailInfo.nSendTime	= (int)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		strncpy(tagMailInfo.strTitle, m_pRow[nColIndex], sizeof(tagMailInfo.strTitle));
		tagMailInfo.strTitle[sizeof(tagMailInfo.strTitle)-1]	= '\0';

		++nColIndex;
		strncpy(tagMailInfo.strContent, m_pRow[nColIndex], sizeof(tagMailInfo.strContent));
		tagMailInfo.strContent[sizeof(tagMailInfo.strContent)-1]	= '\0';

		++nColIndex;
		if (m_pRow[nColIndex])
		{
			nDataLen	= min(sizeof(tagMailInfo.tagMailPlus), pRowLength[nColIndex]);
			memcpy(tagMailInfo.tagMailPlus, m_pRow[nColIndex], nDataLen);
			tagMailInfo.byPlusCount	= nDataLen / sizeof(SMailPlus);
		}
		else
		{
			tagMailInfo.byPlusCount	= 0;
		}

		++nColIndex;
		tagMailInfo.nExpireTime	= (int)strtoul(m_pRow[nColIndex], NULL, 10);

		g_pICenterServerLogic->AddGlobalMail(&tagMailInfo);
	}

	mysql_free_result(m_pQueryRes);

	return true;
}

bool CRoleDBThread::LoadAllGSData()
{
	int	nStrLen	= snprintf(m_strSQL, sizeof(m_strSQL), PROCEDURE_LOAD_ALL_GAME_SERVER);

	if (nStrLen <= 0 || nStrLen >= sizeof(m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] nStrLen Is Error:[%d]\n", __FILE__, __LINE__, nStrLen);
		return false;
	}

	if (!Query(m_strSQL, nStrLen))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return false;
	}

	m_pQueryRes = mysql_use_result(m_pDBHandle);
	if (!m_pQueryRes)
	{
		g_pFileLog->WriteLog("%s[%d] mysql_use_result Failed\n", __FILE__, __LINE__);
		return false;
	}

	int nColIndex;

	while (NULL != (m_pRow = mysql_fetch_row(m_pQueryRes)))
	{
		unsigned long	*pRowLength	= mysql_fetch_lengths(m_pQueryRes);
		if (!pRowLength)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_fetch_lengths Failed\n", __FILE__, __LINE__);
			continue;
		}

		SGSVerifyData	tagGSData	= {0};


		nColIndex	= 0;
		tagGSData.wID	= (WORD)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		strncpy(tagGSData.strName, m_pRow[nColIndex], sizeof(tagGSData.strName));
		tagGSData.strName[sizeof(tagGSData.strName)-1]	= '\0';

		++nColIndex;
		tagGSData.uIP	= (UINT)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		tagGSData.nLast5ClockResetTime	= (int)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		tagGSData.nLastPerDayResetTime	= (int)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		tagGSData.nLastPVPAwardTime	= (int)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		tagGSData.wPlayerCount	= (WORD)strtoul(m_pRow[nColIndex], NULL, 10);

		g_pICenterServerLogic->AddGSData(&tagGSData);
	}

	mysql_free_result(m_pQueryRes);

	return true;
}

bool CRoleDBThread::LoadGSData(const WORD wGameServerID)
{
	int	nStrLen	= snprintf(m_strSQL, sizeof(m_strSQL), PROCEDURE_LOAD_GAME_SERVER, wGameServerID);

	if (nStrLen <= 0 || nStrLen >= sizeof(m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] nStrLen Is Error:[%d]\n", __FILE__, __LINE__, nStrLen);
		return false;
	}

	if (!Query(m_strSQL, nStrLen))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return false;
	}

	m_pQueryRes = mysql_store_result(m_pDBHandle);
	if (NULL == m_pQueryRes)
	{
		g_pFileLog->WriteLog("%s[%d] mysql_store_result Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (1 != mysql_num_rows(m_pQueryRes))
	{
		g_pFileLog->WriteLog("%s[%d] GameServer[%hu] Data Error\n", __FILE__, __LINE__, wGameServerID);
		return false;
	}

	m_pRow = mysql_fetch_row(m_pQueryRes);
	if (NULL == m_pRow)
	{
		g_pFileLog->WriteLog("%s[%d] mysql_fetch_row Failed\n", __FILE__, __LINE__);
		mysql_free_result(m_pQueryRes);
		return false;
	}

	unsigned long	*pRowLength	= mysql_fetch_lengths(m_pQueryRes);
	if (!pRowLength)
	{
		g_pFileLog->WriteLog("%s[%d] mysql_fetch_lengths Failed\n", __FILE__, __LINE__);
		return false;
	}

	int nColIndex;

	SGSVerifyData	tagGSData	= {0};

	nColIndex	= 0;
	tagGSData.wID	= (WORD)strtoul(m_pRow[nColIndex], NULL, 10);

	++nColIndex;
	strncpy(tagGSData.strName, m_pRow[nColIndex], sizeof(tagGSData.strName));
	tagGSData.strName[sizeof(tagGSData.strName)-1]	= '\0';

	++nColIndex;
	tagGSData.uIP	= (UINT)strtoul(m_pRow[nColIndex], NULL, 10);

	++nColIndex;
	tagGSData.nLast5ClockResetTime	= (int)strtoul(m_pRow[nColIndex], NULL, 10);

	++nColIndex;
	tagGSData.nLastPerDayResetTime	= (int)strtoul(m_pRow[nColIndex], NULL, 10);

	++nColIndex;
	tagGSData.nLastPVPAwardTime	= (int)strtoul(m_pRow[nColIndex], NULL, 10);

	++nColIndex;
	tagGSData.wPlayerCount	= (WORD)strtoul(m_pRow[nColIndex], NULL, 10);

	g_pICenterServerLogic->AddGSData(&tagGSData);

	mysql_free_result(m_pQueryRes);

	return true;
}

bool CRoleDBThread::LoadAllActivityData()
{
	int	nStrLen	= snprintf(m_strSQL, sizeof(m_strSQL), PROCEDURE_LOAD_ALL_ACTIVITY);

	if (nStrLen <= 0 || nStrLen >= sizeof(m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] nStrLen Is Error:[%d]\n", __FILE__, __LINE__, nStrLen);
		return false;
	}

	if (!Query(m_strSQL, nStrLen))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return false;
	}

	m_pQueryRes = mysql_use_result(m_pDBHandle);
	if (!m_pQueryRes)
	{
		g_pFileLog->WriteLog("%s[%d] mysql_use_result Failed\n", __FILE__, __LINE__);
		return false;
	}

	int nColIndex;

	while (NULL != (m_pRow = mysql_fetch_row(m_pQueryRes)))
	{
		unsigned long	*pRowLength	= mysql_fetch_lengths(m_pQueryRes);
		if (!pRowLength)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_fetch_lengths Failed\n", __FILE__, __LINE__);
			continue;
		}

		SActivityVerifyData	tagASData	= {0};

		nColIndex	= 0;
		tagASData.wID	= (WORD)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		strncpy(tagASData.strName, m_pRow[nColIndex], sizeof(tagASData.strName));
		tagASData.strName[sizeof(tagASData.strName)-1]	= '\0';

		++nColIndex;
		tagASData.uIP	= (UINT)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		tagASData.nLastPVPAwardTime	= (int)strtoul(m_pRow[nColIndex], NULL, 10);

		g_pICenterServerLogic->AddActivityData(&tagASData);
	}

	mysql_free_result(m_pQueryRes);

	return true;
}

bool CRoleDBThread::LoadActivityData(const WORD wActivityServerID)
{
	int	nStrLen	= snprintf(m_strSQL, sizeof(m_strSQL), PROCEDURE_LOAD_ACTIVITY, wActivityServerID);

	if (nStrLen <= 0 || nStrLen >= sizeof(m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] nStrLen Is Error:[%d]\n", __FILE__, __LINE__, nStrLen);
		return false;
	}

	if (!Query(m_strSQL, nStrLen))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return false;
	}

	m_pQueryRes = mysql_store_result(m_pDBHandle);
	if (NULL == m_pQueryRes)
	{
		g_pFileLog->WriteLog("%s[%d] mysql_store_result Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (1 != mysql_num_rows(m_pQueryRes))
	{
		g_pFileLog->WriteLog("%s[%d] Activity Server[%hu] Data Error\n", __FILE__, __LINE__, wActivityServerID);
		return false;
	}

	m_pRow = mysql_fetch_row(m_pQueryRes);
	if (NULL == m_pRow)
	{
		g_pFileLog->WriteLog("%s[%d] mysql_fetch_row Failed\n", __FILE__, __LINE__);
		mysql_free_result(m_pQueryRes);
		return false;
	}

	unsigned long	*pRowLength	= mysql_fetch_lengths(m_pQueryRes);
	if (!pRowLength)
	{
		g_pFileLog->WriteLog("%s[%d] mysql_fetch_lengths Failed\n", __FILE__, __LINE__);
		return false;
	}

	int nColIndex;

	SActivityVerifyData	tagASData	= {0};

	nColIndex	= 0;
	tagASData.wID	= (WORD)strtoul(m_pRow[nColIndex], NULL, 10);

	++nColIndex;
	strncpy(tagASData.strName, m_pRow[nColIndex], sizeof(tagASData.strName));
	tagASData.strName[sizeof(tagASData.strName)-1]	= '\0';

	++nColIndex;
	tagASData.uIP	= (UINT)strtoul(m_pRow[nColIndex], NULL, 10);

	++nColIndex;
	tagASData.nLastPVPAwardTime	= (int)strtoul(m_pRow[nColIndex], NULL, 10);

	g_pICenterServerLogic->AddActivityData(&tagASData);

	mysql_free_result(m_pQueryRes);

	return true;
}

bool CRoleDBThread::LoadAllGameMasterData()
{
	int	nStrLen	= snprintf(m_strSQL, sizeof(m_strSQL), PROCEDURE_LOAD_ALL_GAME_MASTER);

	if (nStrLen <= 0 || nStrLen >= sizeof(m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] nStrLen Is Error:[%d]\n", __FILE__, __LINE__, nStrLen);
		return false;
	}

	if (!Query(m_strSQL, nStrLen))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return false;
	}

	m_pQueryRes = mysql_use_result(m_pDBHandle);
	if (!m_pQueryRes)
	{
		g_pFileLog->WriteLog("%s[%d] mysql_use_result Failed\n", __FILE__, __LINE__);
		return false;
	}

	int nColIndex;

	while (NULL != (m_pRow = mysql_fetch_row(m_pQueryRes)))
	{
		unsigned long	*pRowLength	= mysql_fetch_lengths(m_pQueryRes);
		if (!pRowLength)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_fetch_lengths Failed\n", __FILE__, __LINE__);
			continue;
		}

		SGMVerfyData	tagGMData;
		memset(&tagGMData, 0, sizeof(tagGMData));

		nColIndex	= 0;
		strncpy(tagGMData.strAccount, m_pRow[nColIndex], sizeof(tagGMData.strAccount));
		tagGMData.strAccount[sizeof(tagGMData.strAccount)-1]	= '\0';

		++nColIndex;
		strncpy(tagGMData.strPassword, m_pRow[nColIndex], sizeof(tagGMData.strPassword));
		tagGMData.strPassword[sizeof(tagGMData.strPassword)-1]	= '\0';

		g_pICenterServerLogic->AddGameMaster(&tagGMData);
	}

	mysql_free_result(m_pQueryRes);

	return true;
}

bool CRoleDBThread::LoadGameMasterData(char (&strAccount)[MAX_ACCOUNT_LEN])
{
	int	nStrLen	= snprintf(m_strSQL, sizeof(m_strSQL), PROCEDURE_LOAD_GAME_MASTER, strAccount);

	if (nStrLen <= 0 || nStrLen >= sizeof(m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] nStrLen Is Error:[%d]\n", __FILE__, __LINE__, nStrLen);
		return false;
	}

	if (!Query(m_strSQL, nStrLen))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return false;
	}

	m_pQueryRes = mysql_store_result(m_pDBHandle);
	if (NULL == m_pQueryRes)
	{
		g_pFileLog->WriteLog("%s[%d] mysql_store_result Failed\n", __FILE__, __LINE__);
		return false;
	}

	if (1 != mysql_num_rows(m_pQueryRes))
	{
		g_pFileLog->WriteLog("[%s][%d] Game Master[%s] Data Error\n", __FILE__, __LINE__, strAccount);
		return false;
	}

	m_pRow = mysql_fetch_row(m_pQueryRes);
	if (NULL == m_pRow)
	{
		g_pFileLog->WriteLog("%s[%d] mysql_fetch_row Failed\n", __FILE__, __LINE__);
		mysql_free_result(m_pQueryRes);
		return false;
	}

	unsigned long	*pRowLength	= mysql_fetch_lengths(m_pQueryRes);
	if (!pRowLength)
	{
		g_pFileLog->WriteLog("%s[%d] mysql_fetch_lengths Failed\n", __FILE__, __LINE__);
		return false;
	}

	int nColIndex;

	SGMVerfyData	tagGMData;
	memset(&tagGMData, 0, sizeof(tagGMData));

	nColIndex	= 0;
	strncpy(tagGMData.strAccount, m_pRow[nColIndex], sizeof(tagGMData.strAccount));
	tagGMData.strAccount[sizeof(tagGMData.strAccount)-1]	= '\0';

	++nColIndex;
	strncpy(tagGMData.strPassword, m_pRow[nColIndex], sizeof(tagGMData.strPassword));
	tagGMData.strPassword[sizeof(tagGMData.strPassword)-1]	= '\0';

	g_pICenterServerLogic->AddGameMaster(&tagGMData);

	mysql_free_result(m_pQueryRes);

	return true;
}

bool CRoleDBThread::CreateRobotRole(const WORD wGameServerID, SCreateRobotInfo &tagCreateRobotInfo)
{
	char			*pszPos		= m_strSQL;
	int				nNowTime	= g_nCenterServerSecond;
	int				nStrLen		= 0;
	unsigned int	uLeftSize	= sizeof(m_strSQL);
	int				nLength		= 0;
	char			strAccount[MAX_ACCOUNT_LEN];
	char			strRoleName[MAX_ROLE_NAME_LEN];

	snprintf(strAccount, sizeof(strAccount), "%03hu%s", wGameServerID, tagCreateRobotInfo.strAccount);
	snprintf(strRoleName, sizeof(strRoleName), "%03hu%s", wGameServerID, tagCreateRobotInfo.strRoleName);
	
	nStrLen = snprintf(pszPos, uLeftSize, "insert into role (Account,RoleName,ServerID,CreateTime,OnlineTime,OfflineTime,Robot,Level,HeroCount,DefendHeroCount,Power,ImageID,Ranking,LastRanking,MagicNum,MagicWeapon,HeroData,DefendLineup,PVPDefendAddInfo,CaveInfo,MagicMatrix) values('");
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return false;
	}

	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	////////////////////
	nLength	= strlen(strAccount);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return false;
		}
		nStrLen = mysql_real_escape_string(m_pDBHandle, pszPos, strAccount, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_real_escape_string SQL Error\n", __FILE__, __LINE__);
			return false;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen = snprintf(pszPos, uLeftSize, "','");
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return false;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;
	////////////////////

	nLength	= strlen(strRoleName);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return false;
		}
		nStrLen = mysql_real_escape_string(m_pDBHandle, pszPos, strRoleName, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_real_escape_string SQL Error\n", __FILE__, __LINE__);
			return false;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen = snprintf(pszPos, uLeftSize, "',%hu,from_unixtime(%d),from_unixtime(%d),from_unixtime(%d),%hhu,%hhu,%hhu,%hhu,%d,%hhu,%hu,%hu,%hhu,'", wGameServerID, nNowTime, nNowTime, nNowTime+1, 1,
		tagCreateRobotInfo.byPlayerLevel, tagCreateRobotInfo.byDefendHeroCount, tagCreateRobotInfo.byDefendHeroCount, tagCreateRobotInfo.nPower, tagCreateRobotInfo.byImagerID, tagCreateRobotInfo.wRanking, 
		tagCreateRobotInfo.wRanking,tagCreateRobotInfo.byOwnMagicNum);
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return false;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;
	////////////////////
	
	nLength	= sizeof(tagCreateRobotInfo.tagMagicData);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return false;
		}

		nStrLen = mysql_real_escape_string(m_pDBHandle, pszPos, (char*)tagCreateRobotInfo.tagMagicData, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_real_escape_string SQL Error\n", __FILE__, __LINE__);
			return false;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen = snprintf(pszPos, uLeftSize, "','");
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return false;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;
	////////////////////

	nLength	= sizeof(tagCreateRobotInfo.tagHeroData);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return false;
		}

		nStrLen = mysql_real_escape_string(m_pDBHandle, pszPos, (char*)tagCreateRobotInfo.tagHeroData, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_real_escape_string SQL Error\n", __FILE__, __LINE__);
			return false;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen = snprintf(pszPos, uLeftSize, "','");
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return false;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;
	////////////////////

	nLength	= tagCreateRobotInfo.byDefendHeroCount * sizeof(SHeroRankingData);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return false;
		}
		nStrLen = mysql_real_escape_string(m_pDBHandle, pszPos, (char*)tagCreateRobotInfo.tagRankingHeroData, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_real_escape_string SQL Error\n", __FILE__, __LINE__);
			return false;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen = snprintf(pszPos, uLeftSize, "','");
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return false;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;
	////////////////////

	nLength	= sizeof(tagCreateRobotInfo.tagDefendAddInfo);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return false;
		}
		nStrLen = mysql_real_escape_string(m_pDBHandle, pszPos, (char*)&tagCreateRobotInfo.tagDefendAddInfo, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_real_escape_string SQL Error\n", __FILE__, __LINE__);
			return false;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen = snprintf(pszPos, uLeftSize, "','");
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return false;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;
	////////////////////

	nLength	= sizeof(tagCreateRobotInfo.tagCave);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return false;
		}
		nStrLen = mysql_real_escape_string(m_pDBHandle, pszPos, (char*)&tagCreateRobotInfo.tagCave, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_real_escape_string SQL Error\n", __FILE__, __LINE__);
			return false;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen = snprintf(pszPos, uLeftSize, "','");
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return false;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;
	////////////////////

	nLength	= sizeof(tagCreateRobotInfo.tagMagicMatrix);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return false;
		}
		nStrLen = mysql_real_escape_string(m_pDBHandle, pszPos, (char*)&tagCreateRobotInfo.tagMagicMatrix, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_real_escape_string SQL Error\n", __FILE__, __LINE__);
			return false;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen = snprintf(pszPos, uLeftSize, "')");
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return false;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;
	////////////////////

	if (!Query(m_strSQL, pszPos-m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return false;
	}

	if (1 != mysql_affected_rows(m_pDBHandle))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return false;
	}

	return true;
}

bool CRoleDBThread::AddGiftCode(SGiftCode *pGiftCode)
{
	char			*pszPos		= m_strSQL;
	unsigned int	uLeftSize	= sizeof(m_strSQL);
	int				nLength		= 0;

	int	nStrLen	= snprintf(m_strSQL, sizeof(m_strSQL), "insert into giftpack (GiftID,GiftCode,StartTime,ExpiredTime,Gift) values(%u,'", pGiftCode->uGiftID);
	if (nStrLen <= 0 || nStrLen >= sizeof(m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return false;
	}

	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= strlen(pGiftCode->strGiftCode);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return false;
		}
		nStrLen	= mysql_real_escape_string(m_pDBHandle, pszPos, pGiftCode->strGiftCode, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
			return false;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen	= snprintf(pszPos, uLeftSize, "',from_unixtime(%d),from_unixtime(%d),'", pGiftCode->nStartTime, pGiftCode->nExpiredTime);
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return false;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= sizeof(pGiftCode->tagItem);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return false;
		}
		nStrLen	= mysql_real_escape_string(m_pDBHandle, pszPos, (char*)pGiftCode->tagItem, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
			return false;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen	= snprintf(pszPos, uLeftSize, "')");
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return false;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	if (!Query(m_strSQL, pszPos-m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return false;
	}

	if (1 != mysql_affected_rows(m_pDBHandle))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return false;
	}

	return true;
}

void CRoleDBThread::DBActive()
{
	if (g_nCenterServerSecond > m_uNextPingTime)
	{
		if (m_pDBHandle)
		{
			mysql_ping(m_pDBHandle);
		}

		m_uNextPingTime	= g_nCenterServerSecond + 60;
	}

	if (m_pDBHandle)
		return;

	if (g_nCenterServerSecond < m_uNextConnectTime)
		return;

	g_pFileLog->WriteLog("Reconnect To Role DB...\n");

	m_pDBHandle	= mysql_init(NULL);
	if (!m_pDBHandle)
	{
		g_pFileLog->WriteLog("%s[%d] m_pDBHandle Is NULL\n", __FILE__, __LINE__);

		m_uNextConnectTime	= g_nCenterServerSecond + 10;

		return;
	}

	char	cReconnectFlag = 1;
	if (0 != mysql_options(m_pDBHandle, MYSQL_OPT_RECONNECT, &cReconnectFlag))
	{
		m_uLastError			= mysql_errno(m_pDBHandle);
		const char	*pstrError	= mysql_error(m_pDBHandle);

		g_pFileLog->WriteLog("%s[%d] mysql_options Error:\n[%u][%s]\n", __FILE__, __LINE__, m_uLastError, pstrError);

		Disconnect();

		m_uNextConnectTime	= g_nCenterServerSecond + 10;

		return;
	}

	if (!mysql_real_connect(m_pDBHandle, m_strDBIP, m_strUserName, m_strPassword, m_strDBName, m_usDBPort, m_strDBName, 0))
	{
		m_uLastError			= mysql_errno(m_pDBHandle);
		const char	*pstrError	= mysql_error(m_pDBHandle);

		g_pFileLog->WriteLog("%s[%d] mysql_real_connect Error:\n[%u][%s]\n", __FILE__, __LINE__, m_uLastError, pstrError);

		Disconnect();

		m_uNextConnectTime	= g_nCenterServerSecond + 10;

		return;
	}

	//if (0 != mysql_autocommit(m_pDBHandle, 0))
	//{
	//	g_pFileLog->WriteLog("%s:%d, mysql_autocommit Close Failed!\n", __FILE__, __LINE__);

	//	Disconnect();

	//	m_uiNextConnectTime	= g_nCenterServerSecond + 10;

	//}

	m_uNextConnectTime	= g_nCenterServerSecond + 10;
}

void CRoleDBThread::ProcessRequest()
{
	UINT		uiPackLen	= 0;
	const void	*pPack		= NULL;

	while(NULL != (pPack = m_pRBRoleDBRequest->RcvPack(uiPackLen)))
	{
		BYTE	byProtocol = *((BYTE*)pPack);

		if (byProtocol >= role_db_end)
		{
			g_pFileLog->WriteLog("%s[%d] Invalid Protocol[%hhu]\n", __FILE__, __LINE__, byProtocol);
			return;
		}

		(this->*m_ProtocolFunc[byProtocol])(pPack, uiPackLen);
	};
}

void CRoleDBThread::RequestCreateRole(const void *pPack, const unsigned int uiPackLen)
{
	ROLE_DB_CREATE_ROLE	*pInfo	= (ROLE_DB_CREATE_ROLE*)pPack;

	char			*pszPos		= m_strSQL;
	int				nStrLen		= 0;
	unsigned int	uLeftSize	= sizeof(m_strSQL);
	int				nLength		= 0;

	nStrLen = snprintf(pszPos, uLeftSize, "call CreateRole('");
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}

	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= strlen(pInfo->strChannelID);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen = mysql_real_escape_string(m_pDBHandle, pszPos, pInfo->strChannelID, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_real_escape_string SQL Error\n", __FILE__, __LINE__);
			return;
		}

		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen = snprintf(pszPos, uLeftSize, "','");
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}

	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= strlen(pInfo->strAccount);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen = mysql_real_escape_string(m_pDBHandle, pszPos, pInfo->strAccount, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_real_escape_string SQL Error\n", __FILE__, __LINE__);
			return;
		}

		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen = snprintf(pszPos, uLeftSize, "',%hu)", pInfo->wGameServerID);
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}

	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	if (!Query(m_strSQL, pszPos-m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return;
	}

	m_pQueryRes = mysql_store_result(m_pDBHandle);
	if (NULL == m_pQueryRes)
	{
		g_pFileLog->WriteLog("%s[%d] mysql_store_result Failed\n", __FILE__, __LINE__);
		return;
	}

	m_pRow = mysql_fetch_row(m_pQueryRes);
	if (NULL == m_pRow)
	{
		g_pFileLog->WriteLog("%s[%d] mysql_fetch_row Failed\n", __FILE__, __LINE__);
		mysql_free_result(m_pQueryRes);
		return;
	}

	unsigned long	*pRowLength	= mysql_fetch_lengths(m_pQueryRes);
	if (!pRowLength)
	{
		g_pFileLog->WriteLog("%s[%d] mysql_fetch_lengths Failed\n", __FILE__, __LINE__);
		return;
	}

	int		nColIndex	= 0;
	BYTE	byResult	= (BYTE)strtoul(m_pRow[nColIndex++], NULL, 10);

	if (byResult)
	{
		RDB_CREATE_ROLE_FAILED	tagFailed;
		tagFailed.byProtocol	= rdb_create_role_failed;
		tagFailed.byResult		= byResult;
		tagFailed.wClientIndex	= pInfo->wClientIndex;
		tagFailed.wGameServerID	= pInfo->wGameServerID;
		tagFailed.wGSNetwork	= pInfo->wGSNetwork;
		m_pRBRoleDBRespond->SndPack(&tagFailed, sizeof(tagFailed));

		mysql_free_result(m_pQueryRes);

		while (!mysql_next_result(m_pDBHandle))
		{
			MYSQL_RES	*pResult = mysql_store_result(m_pDBHandle);
			mysql_free_result(pResult);
		}

		return;
	}

	RDB_RESPOND_ROLE_INFO	tagRespond;
	memset(&tagRespond, 0, sizeof(tagRespond));

	tagRespond.byProtocol				= rdb_respond_role_info;
	tagRespond.wClientIndex				= pInfo->wClientIndex;
	tagRespond.wGSNetwork				= pInfo->wGSNetwork;
	tagRespond.wGameServerID			= pInfo->wGameServerID;

	strncpy(tagRespond.tagRoleInfo.strChannelID, pInfo->strChannelID, sizeof(tagRespond.tagRoleInfo.strChannelID));
	tagRespond.tagRoleInfo.strChannelID[sizeof(tagRespond.tagRoleInfo.strChannelID)-1] = '\0';

	strncpy(tagRespond.tagRoleInfo.strAccount, pInfo->strAccount, sizeof(tagRespond.tagRoleInfo.strAccount));
	tagRespond.tagRoleInfo.strAccount[sizeof(tagRespond.tagRoleInfo.strAccount)-1] = '\0';

	if (m_pRow[nColIndex])
	{
		strncpy(tagRespond.tagRoleInfo.strRoleName, m_pRow[nColIndex], sizeof(tagRespond.tagRoleInfo.strRoleName));
		tagRespond.tagRoleInfo.strRoleName[sizeof(tagRespond.tagRoleInfo.strRoleName)-1] = '\0';
	}
	++nColIndex;

	tagRespond.tagRoleInfo.uiPlayerID	= (UINT)strtoul(m_pRow[nColIndex++], NULL, 10);

	tagRespond.tagRoleInfo.nCreateTime	= (int)strtoul(m_pRow[nColIndex++], NULL, 10);

	tagRespond.tagRoleInfo.nOnlineTime	= (int)strtoul(m_pRow[nColIndex++], NULL, 10);

	tagRespond.tagRoleInfo.nOfflineTime	= (int)strtoul(m_pRow[nColIndex++], NULL, 10);

	tagRespond.tagRoleInfo.uLoginTimes	= (UINT)strtoul(m_pRow[nColIndex++], NULL, 10);

	tagRespond.tagRoleInfo.byRobot		= (BYTE)strtoul(m_pRow[nColIndex++], NULL, 10);

	tagRespond.tagRoleInfo.byGameMaster	= (BYTE)strtoul(m_pRow[nColIndex++], NULL, 10);

	tagRespond.tagRoleInfo.wGuildID		= (WORD)strtoul(m_pRow[nColIndex++], NULL, 10);

	tagRespond.tagRoleInfo.byGuildJob	= (BYTE)strtoul(m_pRow[nColIndex++], NULL, 10);

	tagRespond.tagRoleInfo.nPower		= (int)strtoul(m_pRow[nColIndex++], NULL, 10);

	tagRespond.tagRoleInfo.byImageID	= (BYTE)strtoul(m_pRow[nColIndex++], NULL, 10);

	tagRespond.tagRoleInfo.byHeroCount	= (BYTE)strtoul(m_pRow[nColIndex++], NULL, 10);

	if (m_pRow[nColIndex])
	{
		memcpy(tagRespond.tagRoleInfo.tagHeroData, m_pRow[nColIndex], sizeof(tagRespond.tagRoleInfo.tagHeroData));
	}
	++nColIndex;

	tagRespond.tagRoleInfo.byLevel			= (BYTE)strtoul(m_pRow[nColIndex++], NULL, 10);

	tagRespond.tagRoleInfo.nHeroExpPool		= (int)strtoul(m_pRow[nColIndex++], NULL, 10);

	tagRespond.tagRoleInfo.nDiamond			= (int)strtoul(m_pRow[nColIndex++], NULL, 10);

	tagRespond.tagRoleInfo.uRaffleTicket	= (UINT)strtoul(m_pRow[nColIndex++], NULL, 10);

	tagRespond.tagRoleInfo.uTDMoney			= (UINT)strtoul(m_pRow[nColIndex++], NULL, 10);

	tagRespond.tagRoleInfo.nGold			= (int)strtoul(m_pRow[nColIndex++], NULL, 10);

	tagRespond.tagRoleInfo.nPvPCoin			= (int)strtoul(m_pRow[nColIndex++], NULL, 10);

	tagRespond.tagRoleInfo.nExpeditionCoin	= (int)strtoul(m_pRow[nColIndex++], NULL, 10);

	tagRespond.tagRoleInfo.nLingShi			= (int)strtoul(m_pRow[nColIndex++], NULL, 10);

	tagRespond.tagRoleInfo.nGodSoulsCoin	= (int)strtoul(m_pRow[nColIndex++], NULL, 10);

	tagRespond.tagRoleInfo.nEnergy			= (int)strtoul(m_pRow[nColIndex++], NULL, 10);

	tagRespond.tagRoleInfo.nExp				= (int)strtoul(m_pRow[nColIndex++], NULL, 10);

	tagRespond.tagRoleInfo.byItemCount		= (BYTE)strtoul(m_pRow[nColIndex++], NULL, 10);

	if (m_pRow[nColIndex])
	{
		memcpy(tagRespond.tagRoleInfo.tagItemData, m_pRow[nColIndex], sizeof(tagRespond.tagRoleInfo.tagItemData));
	}
	++nColIndex;

	tagRespond.tagRoleInfo.byResetTimes = (BYTE)strtoul(m_pRow[nColIndex++], NULL, 10);

	if (m_pRow[nColIndex])
	{
		memcpy(tagRespond.tagRoleInfo.tagMapData, m_pRow[nColIndex], sizeof(tagRespond.tagRoleInfo.tagMapData));
	}
	++nColIndex;

	if (m_pRow[nColIndex])
	{
		memcpy(tagRespond.tagRoleInfo.tagMissionData, m_pRow[nColIndex], sizeof(tagRespond.tagRoleInfo.tagMissionData));
	}
	++nColIndex;

	tagRespond.tagRoleInfo.nLotteryScore			= (int)strtoul(m_pRow[nColIndex++], NULL, 10);

	tagRespond.tagRoleInfo.wSkillPoint				= (WORD)strtoul(m_pRow[nColIndex++], NULL, 10);

	tagRespond.tagRoleInfo.nRecvoerSkillPointTime	= (int)strtoul(m_pRow[nColIndex++], NULL, 10);

	tagRespond.tagRoleInfo.byBuySkillPointTimes		= (BYTE)strtoul(m_pRow[nColIndex++], NULL, 10);

	if (m_pRow[nColIndex])
	{
		memcpy(tagRespond.tagRoleInfo.tagEmbattleInfo, m_pRow[nColIndex], sizeof(tagRespond.tagRoleInfo.tagEmbattleInfo));
	}
	++nColIndex;

	tagRespond.tagRoleInfo.byBuyEnergyTimes	= (BYTE)strtoul(m_pRow[nColIndex++], NULL, 10);

	tagRespond.tagRoleInfo.byBuyGoldTimes	= (BYTE)strtoul(m_pRow[nColIndex++], NULL, 10);

	tagRespond.tagRoleInfo.byVipLevel		= (BYTE)strtoul(m_pRow[nColIndex++], NULL, 10);

	if (m_pRow[nColIndex])
	{
		memcpy(tagRespond.tagRoleInfo.byVipGiftSaleState, m_pRow[nColIndex], sizeof(tagRespond.tagRoleInfo.byVipGiftSaleState));
	}
	++nColIndex;

	tagRespond.tagRoleInfo.uRechargeMoney			= (UINT)strtoul(m_pRow[nColIndex++], NULL, 10);	

	tagRespond.tagRoleInfo.uiTotalRechargeDiamond	= (UINT)strtoul(m_pRow[nColIndex++], NULL, 10);	

	tagRespond.tagRoleInfo.uTotalRechargeTimes		= (UINT)strtoul(m_pRow[nColIndex++], NULL, 10);	

	tagRespond.tagRoleInfo.byFirstRechargeAward		= (BYTE)strtoul(m_pRow[nColIndex++], NULL, 10);	

	tagRespond.tagRoleInfo.nLastResetTime			= (int)strtoul(m_pRow[nColIndex++], NULL, 10);

	tagRespond.tagRoleInfo.nLastPerDayResetTime		= (int)strtoul(m_pRow[nColIndex++], NULL, 10);

	tagRespond.tagRoleInfo.uWeekCardDays			= (UINT)strtoul(m_pRow[nColIndex++], NULL, 10);

	tagRespond.tagRoleInfo.uMonthCardDays			= (UINT)strtoul(m_pRow[nColIndex++], NULL, 10);

	tagRespond.tagRoleInfo.uPVPDefendPower			= (UINT)strtoul(m_pRow[nColIndex++], NULL, 10);

	tagRespond.tagRoleInfo.wPVPWinTimes				= (WORD)strtoul(m_pRow[nColIndex++], NULL, 10);

	tagRespond.tagRoleInfo.wRanking					= (WORD)strtoul(m_pRow[nColIndex++], NULL, 10);

	tagRespond.tagRoleInfo.wLastRanking				= (WORD)strtoul(m_pRow[nColIndex++], NULL, 10);

	tagRespond.tagRoleInfo.nNextPVPTime				= (int)strtoul(m_pRow[nColIndex++], NULL, 10);

	tagRespond.tagRoleInfo.byChallengeTimes			= (BYTE)strtoul(m_pRow[nColIndex++], NULL, 10);

	tagRespond.tagRoleInfo.byPVPDefendHeroCount		= (BYTE)strtoul(m_pRow[nColIndex++], NULL, 10);

	if (m_pRow[nColIndex])
	{
		memcpy(tagRespond.tagRoleInfo.tagDefendLineup, m_pRow[nColIndex], sizeof(tagRespond.tagRoleInfo.tagDefendLineup));
	}
	++nColIndex;

	if (m_pRow[nColIndex])
	{
		memcpy(&tagRespond.tagRoleInfo.tagPvpDefendAddInfo, m_pRow[nColIndex], sizeof(tagRespond.tagRoleInfo.tagPvpDefendAddInfo));
	}
	++nColIndex;

	tagRespond.tagRoleInfo.byPVPAttackHeroCount = (BYTE)strtoul(m_pRow[nColIndex++], NULL, 10);

	if (m_pRow[nColIndex])
	{
		memcpy(tagRespond.tagRoleInfo.tagAttackLineup, m_pRow[nColIndex], sizeof(tagRespond.tagRoleInfo.tagAttackLineup));
	}
	++nColIndex;

	if (m_pRow[nColIndex])
	{
		memcpy(&tagRespond.tagRoleInfo.tagPvpAttackAddInfo, m_pRow[nColIndex], sizeof(tagRespond.tagRoleInfo.tagPvpAttackAddInfo));
	}
	++nColIndex;

	if (m_pRow[nColIndex])
	{
		memcpy(tagRespond.tagRoleInfo.tagAchievementData, m_pRow[nColIndex], sizeof(tagRespond.tagRoleInfo.tagAchievementData));
	}
	++nColIndex;

	tagRespond.tagRoleInfo.wAchieveValue			= (UINT)strtoul(m_pRow[nColIndex++], NULL, 10);

	tagRespond.tagRoleInfo.byAchieveAwardCount		= (UINT)strtoul(m_pRow[nColIndex++], NULL, 10);

	tagRespond.tagRoleInfo.uiGoldLotteryTime		= (UINT)strtoul(m_pRow[nColIndex++], NULL, 10);

	tagRespond.tagRoleInfo.uiDiamondLotteryTime		= (UINT)strtoul(m_pRow[nColIndex++], NULL, 10);

	tagRespond.tagRoleInfo.byGoldLotteryTimes		= (BYTE)strtoul(m_pRow[nColIndex++], NULL, 10);

	tagRespond.tagRoleInfo.uiDiamondLotteryTenTimes	= (UINT)strtoul(m_pRow[nColIndex++], NULL, 10);

	if (m_pRow[nColIndex])
	{
		memcpy(tagRespond.tagRoleInfo.tagDailyTask, m_pRow[nColIndex], sizeof(tagRespond.tagRoleInfo.tagDailyTask));
	}
	++nColIndex;

	tagRespond.tagRoleInfo.uiDailyTastNextResetTime	= (UINT)strtoul(m_pRow[nColIndex++], NULL, 10);

	tagRespond.tagRoleInfo.wDailyActive				= (WORD)strtoul(m_pRow[nColIndex++], NULL, 10);

	tagRespond.tagRoleInfo.byDailyAwardCount		= (BYTE)strtoul(m_pRow[nColIndex++], NULL, 10);

	tagRespond.tagRoleInfo.byInductStep				= (BYTE)strtoul(m_pRow[nColIndex++], NULL, 10);

	if (m_pRow[nColIndex])
	{
		memcpy(tagRespond.tagRoleInfo.byInductTask, m_pRow[nColIndex], sizeof(tagRespond.tagRoleInfo.byInductTask));
	}
	++nColIndex;

	tagRespond.tagRoleInfo.byOwnMagicNum = (BYTE)strtoul(m_pRow[nColIndex++], NULL, 10);

	if (m_pRow[nColIndex])
	{
		memcpy(tagRespond.tagRoleInfo.tagMagicData, m_pRow[nColIndex], sizeof(tagRespond.tagRoleInfo.tagMagicData));
	}
	++nColIndex;

	if (m_pRow[nColIndex])
	{
		memcpy(tagRespond.tagRoleInfo.tagStoreInfo, m_pRow[nColIndex], sizeof(tagRespond.tagRoleInfo.tagStoreInfo));
	}
	++nColIndex;

	if (m_pRow[nColIndex])
	{
		memcpy(&tagRespond.tagRoleInfo.tagExpedition, m_pRow[nColIndex], sizeof(tagRespond.tagRoleInfo.tagExpedition));
	}
	++nColIndex;

	if (m_pRow[nColIndex])
	{
		memcpy(&tagRespond.tagRoleInfo.tagFriend, m_pRow[nColIndex], sizeof(tagRespond.tagRoleInfo.tagFriend));
	}
	++nColIndex;

	tagRespond.tagRoleInfo.byPVPRecordCount	= pRowLength[nColIndex] / sizeof(SDBPVPRecord);
	if (m_pRow[nColIndex])
	{
		memcpy(&tagRespond.tagRoleInfo.tagPVPRecord, m_pRow[nColIndex], sizeof(tagRespond.tagRoleInfo.tagPVPRecord));
	}
	++nColIndex;

	if (m_pRow[nColIndex])
	{
		memcpy(&tagRespond.tagRoleInfo.tagHeroPalace, m_pRow[nColIndex], sizeof(tagRespond.tagRoleInfo.tagHeroPalace));
	}
	++nColIndex;

	if (m_pRow[nColIndex])
	{
		memcpy(&tagRespond.tagRoleInfo.tagCave, m_pRow[nColIndex], sizeof(tagRespond.tagRoleInfo.tagCave));
	}
	++nColIndex;

	if (m_pRow[nColIndex])
	{
		memcpy(&tagRespond.tagRoleInfo.bySignInTab, m_pRow[nColIndex], sizeof(tagRespond.tagRoleInfo.bySignInTab));
	}
	++nColIndex;

	if (m_pRow[nColIndex])
	{
		memcpy(&tagRespond.tagRoleInfo.byFeatureState, m_pRow[nColIndex], sizeof(tagRespond.tagRoleInfo.byFeatureState));
	}
	++nColIndex;

	if (m_pRow[nColIndex])
	{
		memcpy(&tagRespond.tagRoleInfo.tagMagicMatrix, m_pRow[nColIndex], sizeof(tagRespond.tagRoleInfo.tagMagicMatrix));
	}
	++nColIndex;

	if (m_pRow[nColIndex])
	{
		memcpy(&tagRespond.tagRoleInfo.tagTowerDefence, m_pRow[nColIndex], sizeof(tagRespond.tagRoleInfo.tagTowerDefence));
	}
	++nColIndex;

	if (m_pRow[nColIndex])
	{
		memcpy(&tagRespond.tagRoleInfo.tagTrailTomb, m_pRow[nColIndex], sizeof(tagRespond.tagRoleInfo.tagTrailTomb));
	}
	++nColIndex;

	if (m_pRow[nColIndex])
	{
		memcpy(&tagRespond.tagRoleInfo.tagClimbingTower, m_pRow[nColIndex], sizeof(tagRespond.tagRoleInfo.tagClimbingTower));
	}
	++nColIndex;

	tagRespond.tagRoleInfo.uCTRanking				= (UINT)strtoul(m_pRow[nColIndex++], NULL, 10);

	tagRespond.tagRoleInfo.uOnlinePVPPoint			= (UINT)strtoul(m_pRow[nColIndex++], NULL, 10);

	tagRespond.tagRoleInfo.uOnlinePVPLastRanking	= (UINT)strtoul(m_pRow[nColIndex++], NULL, 10);

	tagRespond.tagRoleInfo.uOnlinePVPFightCount		= (UINT)strtoul(m_pRow[nColIndex++], NULL, 10);

	tagRespond.tagRoleInfo.uOnlinePVPWinCount		= (UINT)strtoul(m_pRow[nColIndex++], NULL, 10);

	tagRespond.tagRoleInfo.byOnlinePVPLastTitle		= (UINT)strtoul(m_pRow[nColIndex++], NULL, 10);

	if (m_pRow[nColIndex])
	{
		memcpy(&tagRespond.tagRoleInfo.tagOnlinePVP, m_pRow[nColIndex], sizeof(tagRespond.tagRoleInfo.tagOnlinePVP));
	}
	++nColIndex;

	if (m_pRow[nColIndex])
	{
		memcpy(&tagRespond.tagRoleInfo.tagCustomData, m_pRow[nColIndex], sizeof(tagRespond.tagRoleInfo.tagCustomData));
	}
	++nColIndex;

	if (m_pRow[nColIndex])
	{
		memcpy(&tagRespond.tagRoleInfo.uRechargeTimes, m_pRow[nColIndex], sizeof(tagRespond.tagRoleInfo.uRechargeTimes));
	}
	++nColIndex;

	tagRespond.tagRoleInfo.wMainLineId		= (WORD)strtoul(m_pRow[nColIndex++], NULL, 10);

	tagRespond.tagRoleInfo.byCurMainLineDoTimes = (BYTE)strtoul(m_pRow[nColIndex++], NULL, 10);

	if (m_pRow[nColIndex])
	{
		memcpy(&tagRespond.tagRoleInfo.byMainLineState, m_pRow[nColIndex], sizeof(tagRespond.tagRoleInfo.byMainLineState));
	}
	++nColIndex;

	m_pRBRoleDBRespond->SndPack(&tagRespond, sizeof(tagRespond));

	mysql_free_result(m_pQueryRes);

	while (!mysql_next_result(m_pDBHandle))
	{
		MYSQL_RES	*pResult = mysql_store_result(m_pDBHandle);
		mysql_free_result(pResult);
	}
}

void CRoleDBThread::RequestSaveRole(const void *pPack, const unsigned int uiPackLen)
{
	GS2CS_SAVE_ROLE	*pInfo			= (GS2CS_SAVE_ROLE*)pPack;
	char			*pszPos			= m_strSQL;
	unsigned long	ulLength		= 0;
	my_ulonglong	lAffectedRow	= 0;
	SDBRoleInfo		&tagRoleInfo	= pInfo->tagRoleInfo;
	int				nNowTime		= g_nCenterServerSecond;
	int				nStrLen			= 0;
	unsigned int	uLeftSize		= sizeof(m_strSQL);
	int				nLength			= 0;

	nStrLen = snprintf(pszPos, uLeftSize, "update role set Power=%d,ImageID=%hhu,HeroCount=%hhu,HeroData='", tagRoleInfo.nPower, tagRoleInfo.byImageID, tagRoleInfo.byHeroCount);
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= sizeof(tagRoleInfo.tagHeroData);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)tagRoleInfo.tagHeroData, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_real_escape_string SQL Error\n", __FILE__, __LINE__);
			return;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen =  snprintf(pszPos, uLeftSize, "',Level=%hhu,HeroExpPool=%d,Diamond=%d,RaffleTicket=%u,TDMoney=%u,Gold=%d,PvPCoin=%d,ExpCoin=%d,LingShi=%d,GodSouls=%d,Energy=%d,Exp=%d,ItemCount=%hhu,ItemData='",
		(int)tagRoleInfo.byLevel,tagRoleInfo.nHeroExpPool,tagRoleInfo.nDiamond,tagRoleInfo.uRaffleTicket,tagRoleInfo.uTDMoney,tagRoleInfo.nGold,tagRoleInfo.nPvPCoin,tagRoleInfo.nExpeditionCoin,tagRoleInfo.nLingShi,tagRoleInfo.nGodSoulsCoin,tagRoleInfo.nEnergy,tagRoleInfo.nExp,tagRoleInfo.byItemCount);
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= sizeof(tagRoleInfo.tagItemData);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)tagRoleInfo.tagItemData, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_real_escape_string SQL Error\n", __FILE__, __LINE__);
			return;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen = snprintf(pszPos, uLeftSize, "',ResetTimes=%hhu,MapData='", tagRoleInfo.byResetTimes);
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= sizeof(tagRoleInfo.tagMapData);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)tagRoleInfo.tagMapData, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_real_escape_string SQL Error\n", __FILE__, __LINE__);
			return;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen = snprintf(pszPos, uLeftSize, "',MissionData='");
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= sizeof(tagRoleInfo.tagMissionData);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)tagRoleInfo.tagMissionData, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_real_escape_string SQL Error\n", __FILE__, __LINE__);
			return;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen = snprintf(pszPos, uLeftSize, "',LotteryScore=%d,SkillPoint=%hu,NextSkillPointTime=%d,BuySkillPointTimes=%hhu,OnlineTime=from_unixtime(%d),OfflineTime=from_unixtime(%d),LoginTimes=%u,HeroEmbattleInfo='",
		tagRoleInfo.nLotteryScore, tagRoleInfo.wSkillPoint, tagRoleInfo.nRecvoerSkillPointTime, tagRoleInfo.byBuySkillPointTimes, tagRoleInfo.nOnlineTime, nNowTime, tagRoleInfo.uLoginTimes);

	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= sizeof(tagRoleInfo.tagEmbattleInfo);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)tagRoleInfo.tagEmbattleInfo, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_real_escape_string SQL Error\n", __FILE__, __LINE__);
			return;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen = snprintf(pszPos, uLeftSize, 
		"',BuyEnergyTimes=%hhu,BuyGoldTimes=%hhu,VipLevel=%hhu,LastResetTime=%d,LastPerDayResetTime=%d,WeekCardDays=%u,MonthCardDays=%u,RechargeMoney=%u,TotalRechargeDiamond=%u,TotalRechargeTimes=%u,FirstRecharge=%hhu,VipGiftSaleState='",
		tagRoleInfo.byBuyEnergyTimes,
		tagRoleInfo.byBuyGoldTimes,
		tagRoleInfo.byVipLevel,
		tagRoleInfo.nLastResetTime,
		tagRoleInfo.nLastPerDayResetTime,
		tagRoleInfo.uWeekCardDays,
		tagRoleInfo.uMonthCardDays,
		tagRoleInfo.uRechargeMoney,
		tagRoleInfo.uiTotalRechargeDiamond,
		tagRoleInfo.uTotalRechargeTimes,
		tagRoleInfo.byFirstRechargeAward);
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= sizeof(tagRoleInfo.byVipGiftSaleState);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)tagRoleInfo.byVipGiftSaleState, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_real_escape_string SQL Error\n", __FILE__, __LINE__);
			return;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen = snprintf(pszPos, uLeftSize, "',PVPDefendPower=%u,PVPWinTimes=%hu,Ranking=%hu,LastRanking=%hu,NextPVPTime=%d,ChallengeTimes=%hhu,DefendHeroCount=%hhu,DefendLineup='",
		tagRoleInfo.uPVPDefendPower,tagRoleInfo.wPVPWinTimes,tagRoleInfo.wRanking, tagRoleInfo.wLastRanking,tagRoleInfo.nNextPVPTime, tagRoleInfo.byChallengeTimes, tagRoleInfo.byPVPDefendHeroCount);
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= sizeof(tagRoleInfo.tagDefendLineup);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)tagRoleInfo.tagDefendLineup, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_real_escape_string SQL Error\n", __FILE__, __LINE__);
			return;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen = snprintf(pszPos, uLeftSize, "',PVPDefendAddInfo='");
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= sizeof(tagRoleInfo.tagPvpDefendAddInfo);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)&tagRoleInfo.tagPvpDefendAddInfo, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_real_escape_string SQL Error\n", __FILE__, __LINE__);
			return;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}


	nStrLen = snprintf(pszPos, uLeftSize, "',PvpAttackAddInfo='");
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= sizeof(tagRoleInfo.tagPvpAttackAddInfo);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)&tagRoleInfo.tagPvpAttackAddInfo, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_real_escape_string SQL Error\n", __FILE__, __LINE__);
			return;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}


	nStrLen = snprintf(pszPos, uLeftSize, "',AttackHeroCount=%hhu,AttackLineup='", tagRoleInfo.byPVPAttackHeroCount);
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= sizeof(tagRoleInfo.tagAttackLineup);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)tagRoleInfo.tagAttackLineup, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_real_escape_string SQL Error\n", __FILE__, __LINE__);
			return;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen = snprintf(pszPos, uLeftSize, "',AchieveValue=%hu,AchieveAwardCount=%hhu,AchievementData='",tagRoleInfo.wAchieveValue,tagRoleInfo.byAchieveAwardCount);
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= sizeof(tagRoleInfo.tagAchievementData);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)tagRoleInfo.tagAchievementData, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_real_escape_string SQL Error\n", __FILE__, __LINE__);
			return;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen = snprintf(pszPos, uLeftSize, "',GoldLotteryTime=%u,DiamondLotteryTime=%u,GoldLotteryTimes=%hhu,DiamondLotteryTenTimes=%u,DailyTastNextResetTime=%u,DailyActive=%hu,DailyAwardCount=%hhu,DailyTask ='",
		tagRoleInfo.uiGoldLotteryTime,tagRoleInfo.uiDiamondLotteryTime,tagRoleInfo.byGoldLotteryTimes,tagRoleInfo.uiDiamondLotteryTenTimes,tagRoleInfo.uiDailyTastNextResetTime,tagRoleInfo.wDailyActive,tagRoleInfo.byDailyAwardCount);		   
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= sizeof(tagRoleInfo.tagDailyTask);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)tagRoleInfo.tagDailyTask, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_real_escape_string SQL Error\n", __FILE__, __LINE__);
			return;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen = snprintf(pszPos, uLeftSize, "',InductStep=%hhu,InductTask='",tagRoleInfo.byInductStep);
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= sizeof(tagRoleInfo.byInductTask);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)tagRoleInfo.byInductTask, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_real_escape_string SQL Error\n", __FILE__, __LINE__);
			return;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen = snprintf(pszPos, uLeftSize, "',MagicNum=%hhu,MagicWeapon='", tagRoleInfo.byOwnMagicNum);
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= sizeof(tagRoleInfo.tagMagicData);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)tagRoleInfo.tagMagicData, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_real_escape_string SQL Error\n", __FILE__, __LINE__);
			return;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen = snprintf(pszPos, uLeftSize, "',StoreInfo='");
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= sizeof(tagRoleInfo.tagStoreInfo);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)tagRoleInfo.tagStoreInfo, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_real_escape_string SQL Error\n", __FILE__, __LINE__);
			return;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen = snprintf(pszPos, uLeftSize, "',Expedition='");
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= sizeof(tagRoleInfo.tagExpedition);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)&tagRoleInfo.tagExpedition, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_real_escape_string SQL Error\n", __FILE__, __LINE__);
			return;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen = snprintf(pszPos, uLeftSize, "',Friend='");
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= sizeof(tagRoleInfo.tagFriend);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)&tagRoleInfo.tagFriend, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_real_escape_string SQL Error\n", __FILE__, __LINE__);
			return;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen = snprintf(pszPos, uLeftSize, "',PVPRecord='");
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= sizeof(SDBPVPRecord) * (min(tagRoleInfo.byPVPRecordCount,MAX_PVP_RECORD_COUNT));
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)&tagRoleInfo.tagPVPRecord, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_real_escape_string SQL Error\n", __FILE__, __LINE__);
			return;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen = snprintf(pszPos, uLeftSize, "',SignInTab='");
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= sizeof(tagRoleInfo.bySignInTab);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)&tagRoleInfo.bySignInTab, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_real_escape_string SQL Error\n", __FILE__, __LINE__);
			return;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen = snprintf(pszPos, uLeftSize, "',FeatureState='");
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= sizeof(tagRoleInfo.byFeatureState);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)&tagRoleInfo.byFeatureState, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_real_escape_string SQL Error\n", __FILE__, __LINE__);
			return;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen = snprintf(pszPos, uLeftSize, "',MagicMatrix='");
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= sizeof(tagRoleInfo.tagMagicMatrix);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)&tagRoleInfo.tagMagicMatrix, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_real_escape_string SQL Error\n", __FILE__, __LINE__);
			return;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen = snprintf(pszPos, uLeftSize, "',TowerDefence='");
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= sizeof(tagRoleInfo.tagTowerDefence);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)&tagRoleInfo.tagTowerDefence, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_real_escape_string SQL Error\n", __FILE__, __LINE__);
			return;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen = snprintf(pszPos, uLeftSize, "',TrailTomb='");
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= sizeof(tagRoleInfo.tagTrailTomb);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)&tagRoleInfo.tagTrailTomb, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_real_escape_string SQL Error\n", __FILE__, __LINE__);
			return;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen = snprintf(pszPos, uLeftSize, "',ClimbingTower='");
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= sizeof(tagRoleInfo.tagClimbingTower);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)&tagRoleInfo.tagClimbingTower, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_real_escape_string SQL Error\n", __FILE__, __LINE__);
			return;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen = snprintf(pszPos, uLeftSize, "',OnlinePVP='");
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= sizeof(tagRoleInfo.tagOnlinePVP);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)&tagRoleInfo.tagOnlinePVP, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_real_escape_string SQL Error\n", __FILE__, __LINE__);
			return;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen = snprintf(pszPos, uLeftSize, "',CustomData='");
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= sizeof(tagRoleInfo.tagCustomData);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)&tagRoleInfo.tagCustomData, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_real_escape_string SQL Error\n", __FILE__, __LINE__);
			return;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen = snprintf(pszPos, uLeftSize, "',RechargeTimes='");
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= sizeof(tagRoleInfo.uRechargeTimes);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)&tagRoleInfo.uRechargeTimes, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_real_escape_string SQL Error\n", __FILE__, __LINE__);
			return;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen = snprintf(pszPos, uLeftSize, "',HeroPalace='");
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= sizeof(tagRoleInfo.tagHeroPalace);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)&tagRoleInfo.tagHeroPalace, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_real_escape_string SQL Error\n", __FILE__, __LINE__);
			return;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen = snprintf(pszPos, uLeftSize, "',CaveInfo='");
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= sizeof(tagRoleInfo.tagCave);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)&tagRoleInfo.tagCave, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_real_escape_string SQL Error\n", __FILE__, __LINE__);
			return;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen = snprintf(pszPos, uLeftSize, "',MainLineId=%hu,MainLineDoTimes=%hhu,MainLineState='", tagRoleInfo.wMainLineId, tagRoleInfo.byCurMainLineDoTimes);
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= sizeof(tagRoleInfo.byMainLineState);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)tagRoleInfo.byMainLineState, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_real_escape_string SQL Error\n", __FILE__, __LINE__);
			return;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen = snprintf(pszPos, uLeftSize, "' where PlayerID=%u", tagRoleInfo.uiPlayerID);
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	if (!Query(m_strSQL, pszPos-m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return;
	}

	lAffectedRow	= mysql_affected_rows(m_pDBHandle);
	if (lAffectedRow > 1)
	{
		g_pFileLog->WriteLog("%s[%d] Save Role Failed:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return;
	}
}

void CRoleDBThread::RequestRename(const void *pPack, const unsigned int uiPackLen)
{
	GS2CS_ROLE_RENAME	*pInfo			= (GS2CS_ROLE_RENAME*)pPack;
	char				*pszPos			= m_strSQL;
	unsigned long		ulLength		= 0;
	my_ulonglong		lAffectedRow	= 0;
	int					nStrLen			= 0;
	unsigned int		uLeftSize		= sizeof(m_strSQL);
	int					nLength			= 0;

	pInfo->strRoleName[sizeof(pInfo->strRoleName)-1]	= '\0';

	nStrLen = snprintf(pszPos, uLeftSize, "call RoleRename('");
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= strlen(pInfo->strRoleName);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen = mysql_real_escape_string(m_pDBHandle, pszPos, pInfo->strRoleName, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
			return;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen = snprintf(pszPos, uLeftSize, "',%u)", pInfo->uPlayerID);
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	if (!Query(m_strSQL, pszPos-m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return;
	}

	m_pQueryRes = mysql_store_result(m_pDBHandle);
	if (NULL == m_pQueryRes)
	{
		g_pFileLog->WriteLog("%s[%d] mysql_store_result Failed\n", __FILE__, __LINE__);
		return;
	}

	if (1 != mysql_num_rows(m_pQueryRes))
	{
		g_pFileLog->WriteLog("[%s][%d] Result Data Error\n", __FILE__, __LINE__);
		mysql_free_result(m_pQueryRes);
		return;
	}

	m_pRow = mysql_fetch_row(m_pQueryRes);
	if (NULL == m_pRow)
	{
		g_pFileLog->WriteLog("%s[%d] mysql_fetch_row Failed\n", __FILE__, __LINE__);
		mysql_free_result(m_pQueryRes);
		return;
	}

	unsigned long	*pRowLength	= mysql_fetch_lengths(m_pQueryRes);
	if (!pRowLength)
	{
		g_pFileLog->WriteLog("%s[%d] mysql_fetch_lengths Failed\n", __FILE__, __LINE__);
		mysql_free_result(m_pQueryRes);
		return;
	}

	RDB_ROLE_RENAME_RESULT	tagInfo;
	memset(&tagInfo, 0, sizeof(tagInfo));

	int		nColIndex	= 0;
	tagInfo.byResult	= (BYTE)strtoul(m_pRow[nColIndex++], NULL, 10);

	strncpy(tagInfo.strRoleName, m_pRow[nColIndex++], sizeof(tagInfo.strRoleName));
	tagInfo.strRoleName[sizeof(tagInfo.strRoleName)-1]	= '\0';

	tagInfo.byProtocol	= rdb_role_rename_result;
	tagInfo.wServerID	= pInfo->wServerID;
	tagInfo.uPlayerID	= pInfo->uPlayerID;
	tagInfo.byCostMoney	= pInfo->byCostMoney;

	m_pRBRoleDBRespond->SndPack(&tagInfo, sizeof(tagInfo));

	mysql_free_result(m_pQueryRes);

	while (!mysql_next_result(m_pDBHandle))
	{
		MYSQL_RES	*pResult = mysql_store_result(m_pDBHandle);
		mysql_free_result(pResult);
	}
}

void CRoleDBThread::RequestCreateMail(const void *pPack, const unsigned int uiPackLen)
{
	GS2CS_SEND_MAIL	*pInfo			= (GS2CS_SEND_MAIL*)pPack;
	char			*pMailContent	= (char*)((char*)pPack+sizeof(GS2CS_SEND_MAIL));
	SMailPlus		*pMailPlus		= (SMailPlus*)((char*)pPack+sizeof(GS2CS_SEND_MAIL)+pInfo->wContentLen);
	int				nPostTime		= g_nCenterServerSecond;
	char			*pszPos			= m_strSQL;
	int				nStrLen			= 0;
	unsigned int	uLeftSize		= sizeof(m_strSQL);
	int				nLength			= 0;

	nStrLen = snprintf(pszPos, uLeftSize, "insert into mails (ReceiverID,PostTime,ExpireTime,Type,Title,Content,MailData) values(%u,from_unixtime(%d),from_unixtime(%d),%hhu,'",
		pInfo->uiReceiverID,
		nPostTime,
		nPostTime+pInfo->nExpireTime,
		pInfo->byType);
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= strlen(pInfo->strTitle);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen = mysql_real_escape_string(m_pDBHandle, pszPos, pInfo->strTitle, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
			return;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen = snprintf(pszPos, uLeftSize, "','");
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= pInfo->wContentLen;
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen = mysql_real_escape_string(m_pDBHandle, pszPos, pMailContent, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
			return;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen = snprintf(pszPos, uLeftSize, "','");
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= pInfo->byPlusCount*sizeof(SMailPlus);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen = mysql_real_escape_string(m_pDBHandle, pszPos, (char*)(pMailPlus), nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
			return;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen = snprintf(pszPos, uLeftSize, "')");
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	if (!Query(m_strSQL, pszPos-m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return;
	}

	if (1 != mysql_affected_rows(m_pDBHandle))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return;
	}

	LoadNewMail(pInfo, nPostTime);
}

void CRoleDBThread::LoadNewMail(GS2CS_SEND_MAIL *pLoadInfo, const int nPostTime)
{
	int	nStrLen	= snprintf(m_strSQL, sizeof(m_strSQL), PROCEDURE_LOAD_NEW_MAIL, pLoadInfo->uiReceiverID, nPostTime);
	if (nStrLen <= 0 || nStrLen >= sizeof(m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] nStrLen[%d] Is Error\n", __FILE__, __LINE__, nStrLen);
		return;
	}

	if (!Query(m_strSQL, nStrLen))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return;
	}

	m_pQueryRes = mysql_use_result(m_pDBHandle);
	if (!m_pQueryRes)
	{
		g_pFileLog->WriteLog("%s[%d] mysql_use_result Failed\n", __FILE__, __LINE__);
		return;
	}

	int						nColIndex	= 0;
	int						nDataLen	= 0;
	BYTE					byMailCount	= 0;
	char					strBuff[MAX_MAIL_DATA_LENGTH];
	RDB_RESPOND_NEW_MAIL	*pMailListInfo	= (RDB_RESPOND_NEW_MAIL*)strBuff;
	SMailInfo				*pMailBaseInfo	= (SMailInfo*)(strBuff + sizeof(RDB_RESPOND_NEW_MAIL));

	memset(strBuff, 0, sizeof(strBuff));

	pMailListInfo->byProtocol	= rdb_respond_new_mail;
	pMailListInfo->wPlayerIndex	= pLoadInfo->wPlayerIndex;
	pMailListInfo->uiPlayerID	= pLoadInfo->uiReceiverID;

	while (NULL != (m_pRow = mysql_fetch_row(m_pQueryRes)))
	{
		unsigned long	*pRowLength	= mysql_fetch_lengths(m_pQueryRes);
		if (!pRowLength)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_fetch_lengths Failed\n", __FILE__, __LINE__);
			continue;
		}

		nColIndex	= 0;
		pMailBaseInfo->uiMailID		= (UINT)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		pMailBaseInfo->uiReceiverID	= (UINT)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		pMailBaseInfo->byType		= (BYTE)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		pMailBaseInfo->byState		= (BYTE)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		pMailBaseInfo->byPickup		= (BYTE)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		pMailBaseInfo->nSendTime	= (int)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		pMailBaseInfo->nExpireTime	= (int)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		strncpy(pMailBaseInfo->strTitle, m_pRow[nColIndex], sizeof(pMailBaseInfo->strTitle));
		pMailBaseInfo->strTitle[sizeof(pMailBaseInfo->strTitle)-1]	= '\0';

		++nColIndex;
		strncpy(pMailBaseInfo->strContent, m_pRow[nColIndex], sizeof(pMailBaseInfo->strContent));
		pMailBaseInfo->strContent[sizeof(pMailBaseInfo->strContent)-1]	= '\0';

		++nColIndex;
		if (m_pRow[nColIndex])
		{
			nDataLen	= min(sizeof(pMailBaseInfo->tagMailPlus), pRowLength[nColIndex]);
			memcpy(pMailBaseInfo->tagMailPlus, m_pRow[nColIndex], nDataLen);
			pMailBaseInfo->byPlusCount	= nDataLen / sizeof(SMailPlus);
		}
		else
		{
			pMailBaseInfo->byPlusCount	= 0;
		}

		++byMailCount;
		++pMailBaseInfo;
	}

	mysql_free_result(m_pQueryRes);

	pMailListInfo->byMailCount	= byMailCount;
	pMailListInfo->wSize		= sizeof(RDB_RESPOND_NEW_MAIL) - sizeof(SUndefinedProtocolHead) + byMailCount * sizeof(SMailInfo);

	m_pRBRoleDBRespond->SndPack(strBuff, pMailListInfo->wSize + sizeof(SUndefinedProtocolHead));
}

void CRoleDBThread::LoadNewGlobalMail(const int nPostTime)
{
	int	nStrLen	= snprintf(m_strSQL, sizeof(m_strSQL), PROCEDURE_LOAD_NEW_GLOBAL_MAIL, nPostTime);
	if (nStrLen <= 0 || nStrLen >= sizeof(m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] nStrLen[%d] Error\n", __FILE__, __LINE__, nStrLen);
		return;
	}

	if (!Query(m_strSQL, nStrLen))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return;
	}

	m_pQueryRes = mysql_use_result(m_pDBHandle);
	if (!m_pQueryRes)
	{
		g_pFileLog->WriteLog("%s[%d] mysql_use_result Failed\n", __FILE__, __LINE__);
		return;
	}

	int							nColIndex	= 0;
	int							nDataLen	= 0;
	BYTE						byMailCount	= 0;
	char						strBuff[MAX_MAIL_DATA_LENGTH];
	RDB_RESPOND_NEW_GLOBAL_MAIL	*pMailListInfo	= (RDB_RESPOND_NEW_GLOBAL_MAIL*)strBuff;
	SGlobalMailInfo				*pGlobalMail	= (SGlobalMailInfo*)(strBuff + sizeof(RDB_RESPOND_NEW_GLOBAL_MAIL));

	memset(strBuff, 0, sizeof(strBuff));

	pMailListInfo->byProtocol	= rdb_respond_new_global_mail;

	while (NULL != (m_pRow = mysql_fetch_row(m_pQueryRes)))
	{
		unsigned long	*pRowLength	= mysql_fetch_lengths(m_pQueryRes);
		if (!pRowLength)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_fetch_lengths Failed\n", __FILE__, __LINE__);
			continue;
		}

		nColIndex	= 0;
		pGlobalMail->uiMailID	= (UINT)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		pGlobalMail->nSendTime	= (int)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		strncpy(pGlobalMail->strTitle, m_pRow[nColIndex], sizeof(pGlobalMail->strTitle));
		pGlobalMail->strTitle[sizeof(pGlobalMail->strTitle)-1]	= '\0';

		++nColIndex;
		strncpy(pGlobalMail->strContent, m_pRow[nColIndex], sizeof(pGlobalMail->strContent));
		pGlobalMail->strContent[sizeof(pGlobalMail->strContent)-1]	= '\0';

		++nColIndex;
		if (m_pRow[nColIndex])
		{
			nDataLen	= min(sizeof(pGlobalMail->tagMailPlus), pRowLength[nColIndex]);
			memcpy(pGlobalMail->tagMailPlus, m_pRow[nColIndex], nDataLen);
			pGlobalMail->byPlusCount	= nDataLen / sizeof(SMailPlus);
		}
		else
		{
			pGlobalMail->byPlusCount	= 0;
		}

		++nColIndex;
		pGlobalMail->nExpireTime	= (int)strtoul(m_pRow[nColIndex], NULL, 10);


		++byMailCount;
		++pGlobalMail;
	}

	mysql_free_result(m_pQueryRes);

	pMailListInfo->byMailCount	= byMailCount;
	pMailListInfo->wSize		= sizeof(RDB_RESPOND_NEW_GLOBAL_MAIL) - sizeof(SUndefinedProtocolHead) + byMailCount * sizeof(SGlobalMailInfo);

	m_pRBRoleDBRespond->SndPack(strBuff, pMailListInfo->wSize + sizeof(SUndefinedProtocolHead));
}

void CRoleDBThread::RequestLoadMails(const void *pPack, const unsigned int uiPackLen)
{
	GS2CS_LOAD_MAIL_LIST	*pInfo	= (GS2CS_LOAD_MAIL_LIST*)pPack;

	int	nStrLen	= snprintf(m_strSQL, sizeof(m_strSQL), PROCEDURE_LOAD_MAIL_LIST, pInfo->uiPlayerID);
	if (nStrLen <= 0 || nStrLen >= sizeof(m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] nStrLen[%d] Error\n", __FILE__, __LINE__, nStrLen);
		return;
	}

	if (!Query(m_strSQL, nStrLen))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return;
	}

	m_pQueryRes = mysql_use_result(m_pDBHandle);
	if (!m_pQueryRes)
	{
		g_pFileLog->WriteLog("%s[%d] mysql_use_result Failed\n", __FILE__, __LINE__);
		return;
	}

	int						nColIndex	= 0;
	int						nDataLen	= 0;
	BYTE					byMailCount	= 0;
	char					strBuff[MAX_MAIL_DATA_LENGTH];
	RDB_RESPOND_MAIL_LIST	*pMailListInfo	= (RDB_RESPOND_MAIL_LIST*)strBuff;
	SMailInfo				*pMailBaseInfo	= (SMailInfo*)(strBuff + sizeof(RDB_RESPOND_MAIL_LIST));

	memset(strBuff, 0, sizeof(strBuff));

	pMailListInfo->byProtocol	= rdb_respond_mail_list;
	pMailListInfo->wGSNetwork	= pInfo->wGSNetwork;
	pMailListInfo->wPlayerIndex	= pInfo->wPlayerIndex;
	pMailListInfo->uiPlayerID	= pInfo->uiPlayerID;

	while (NULL != (m_pRow = mysql_fetch_row(m_pQueryRes)))
	{
		unsigned long	*pRowLength	= mysql_fetch_lengths(m_pQueryRes);
		if (!pRowLength)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_fetch_lengths Failed\n", __FILE__, __LINE__);
			continue;
		}

		nColIndex	= 0;
		pMailBaseInfo->uiMailID		= (UINT)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		pMailBaseInfo->uiReceiverID	= (UINT)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		pMailBaseInfo->byType		= (BYTE)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		pMailBaseInfo->byState		= (BYTE)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		pMailBaseInfo->byPickup		= (BYTE)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		pMailBaseInfo->nSendTime	= (int)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		pMailBaseInfo->nExpireTime	= (int)strtoul(m_pRow[nColIndex], NULL, 10);

		++nColIndex;
		strncpy(pMailBaseInfo->strTitle, m_pRow[nColIndex], sizeof(pMailBaseInfo->strTitle));
		pMailBaseInfo->strTitle[sizeof(pMailBaseInfo->strTitle)-1]	= '\0';

		++nColIndex;
		strncpy(pMailBaseInfo->strContent, m_pRow[nColIndex], sizeof(pMailBaseInfo->strContent));
		pMailBaseInfo->strContent[sizeof(pMailBaseInfo->strContent)-1]	= '\0';

		++nColIndex;
		if (m_pRow[nColIndex])
		{
			nDataLen	= min(sizeof(pMailBaseInfo->tagMailPlus), pRowLength[nColIndex]);
			memcpy(pMailBaseInfo->tagMailPlus, m_pRow[nColIndex], nDataLen);
			pMailBaseInfo->byPlusCount	= nDataLen / sizeof(SMailPlus);
		}
		else
		{
			pMailBaseInfo->byPlusCount	= 0;
		}

		++byMailCount;
		++pMailBaseInfo;
	}

	mysql_free_result(m_pQueryRes);

	pMailListInfo->byMailCount	= byMailCount;
	pMailListInfo->wSize		= sizeof(RDB_RESPOND_MAIL_LIST) - sizeof(SUndefinedProtocolHead) + byMailCount * sizeof(SMailInfo);

	m_pRBRoleDBRespond->SndPack(strBuff, pMailListInfo->wSize + sizeof(SUndefinedProtocolHead));
}

void CRoleDBThread::RequestUpdateMail(const void *pPack, const unsigned int uiPackLen)
{
	GS2CS_UPDATE_MAIL	*pInfo		= (GS2CS_UPDATE_MAIL*)pPack;
	SMailPlus			*pMailPlus	= (SMailPlus*)((char*)pPack+sizeof(GS2CS_UPDATE_MAIL));
	char				*pszPos		= m_strSQL;
	int					nStrLen		= 0;
	unsigned int		uLeftSize	= sizeof(m_strSQL);
	int					nLength		= 0;

	nStrLen	= snprintf(pszPos, uLeftSize, "update mails set State=%hhu,Pickup=%hhu,MailData='", pInfo->byState, pInfo->byPickup);
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= sizeof(SMailPlus)*pInfo->byPlusCount;
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen	= mysql_real_escape_string(m_pDBHandle, pszPos, (char*)pMailPlus, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
			return;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen	= snprintf(pszPos, uLeftSize, "' where ID=%u", pInfo->uiMailID);
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}

	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	if (!Query(m_strSQL, pszPos-m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return;
	}
}

void CRoleDBThread::RequestDeleteMail(const void *pPack, const unsigned int uiPackLen)
{
	GS2CS_DELETE_MAIL	*pInfo	= (GS2CS_DELETE_MAIL*)pPack;

	int	nStrLen	= snprintf(m_strSQL, sizeof(m_strSQL), PROCEDURE_DELETE_PLAYER_MAIL, pInfo->uiMailID);
	if (nStrLen <= 0 || nStrLen >= sizeof(m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}

	if (!Query(m_strSQL, nStrLen))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return;
	}
}

void CRoleDBThread::RequestCreateGlobalMail(const void *pPack, const unsigned int uiPackLen)
{
	GS2CS_SEND_GLOBAL_MAIL	*pInfo			= (GS2CS_SEND_GLOBAL_MAIL*)pPack;
	char					*pMailContent	= (char*)((char*)pPack+sizeof(GS2CS_SEND_GLOBAL_MAIL));
	SMailPlus				*pMailPlus		= (SMailPlus*)((char*)pPack+sizeof(GS2CS_SEND_GLOBAL_MAIL)+pInfo->wContentLen);
	int						nNowTime		= g_nCenterServerSecond;
	char					*pszPos			= m_strSQL;
	int						nStrLen			= 0;
	unsigned int			uLeftSize		= sizeof(m_strSQL);
	int						nLength			= 0;

	nStrLen = snprintf(pszPos, uLeftSize, "insert into globalmails (PostTime,ExpireTime,Title,Content,MailData) values(from_unixtime(%d),from_unixtime(%d),'", nNowTime, pInfo->nExpireTime+nNowTime);
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= strlen(pInfo->strTitle);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen = mysql_real_escape_string(m_pDBHandle, pszPos, pInfo->strTitle, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
			return;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen = snprintf(pszPos, uLeftSize, "','");
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= pInfo->wContentLen;
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen = mysql_real_escape_string(m_pDBHandle, pszPos, pMailContent, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
			return;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen = snprintf(pszPos, uLeftSize, "','");
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= pInfo->byPlusCount*sizeof(SMailPlus);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen = mysql_real_escape_string(m_pDBHandle, pszPos, (char*)(pMailPlus), nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
			return;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen = snprintf(pszPos, uLeftSize, "')");
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	if (!Query(m_strSQL, pszPos-m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return;
	}

	if (1 != mysql_affected_rows(m_pDBHandle))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return;
	}

	LoadNewGlobalMail(nNowTime);
}

void CRoleDBThread::RequestDeleteExpireMail(const void *pPack, const unsigned int uiPackLen)
{
	DeletePlayerExpireMail();

	DeleteGlobalExpireMail();
}

void CRoleDBThread::DeletePlayerExpireMail()
{
	int	nStrLen	= snprintf(m_strSQL, sizeof(m_strSQL), PROCEDURE_DELETE_PLAYER_EXPIRE_MAIL);
	if (nStrLen <= 0 || nStrLen >= sizeof(m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}

	if (!Query(m_strSQL, nStrLen))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return;
	}
}

void CRoleDBThread::DeleteGlobalExpireMail()
{
	int	nStrLen	= snprintf(m_strSQL, sizeof(m_strSQL), PROCEDURE_DELETE_GLOBAL_EXPIRE_MAIL);
	if (nStrLen <= 0 || nStrLen >= sizeof(m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}

	if (!Query(m_strSQL, nStrLen))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return;
	}
}

void CRoleDBThread::RequestCreateMailOnly(const void *pPack, const unsigned int uiPackLen)
{
	GS2CS_SEND_MAIL	*pInfo			= (GS2CS_SEND_MAIL*)pPack;
	char			*pMailContent	= (char*)((char*)pPack+sizeof(GS2CS_SEND_MAIL));
	SMailPlus		*pMailPlus		= (SMailPlus*)((char*)pPack+sizeof(GS2CS_SEND_MAIL)+pInfo->wContentLen);
	int				nPostTime		= g_nCenterServerSecond;
	char			*pszPos			= m_strSQL;
	int				nStrLen			= 0;
	unsigned int	uLeftSize		= sizeof(m_strSQL);
	int				nLength			= 0;

	nStrLen	= snprintf(pszPos, uLeftSize, "insert into mails (ReceiverID,PostTime,ExpireTime,Type,Title,Content,MailData) values(%u,from_unixtime(%d),from_unixtime(%d),%hhu,'",
		pInfo->uiReceiverID,
		nPostTime,
		nPostTime+pInfo->nExpireTime,
		pInfo->byType);
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= strlen(pInfo->strTitle);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen	= mysql_real_escape_string(m_pDBHandle, pszPos, pInfo->strTitle, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
			return;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen	= snprintf(pszPos, uLeftSize, "','");
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= pInfo->wContentLen;
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen	= mysql_real_escape_string(m_pDBHandle, pszPos, pMailContent, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
			return;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen	= snprintf(pszPos, uLeftSize, "','");
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= pInfo->byPlusCount*sizeof(SMailPlus);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen	= mysql_real_escape_string(m_pDBHandle, pszPos, (char*)(pMailPlus), nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
			return;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen	= snprintf(pszPos, uLeftSize, "')");
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	if (!Query(m_strSQL, pszPos-m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return;
	}

	if (1 != mysql_affected_rows(m_pDBHandle))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return;
	}
}

void CRoleDBThread::RequestUpdateMailExpire(const void *pPack, const unsigned int uiPackLen)
{
	GS2CS_UPDATE_MAIL_EXPIRE	*pInfo	= (GS2CS_UPDATE_MAIL_EXPIRE*)pPack;

	int	nStrLen	= snprintf(m_strSQL, sizeof(m_strSQL), "update mails set ExpireTime=from_unixtime(%d) where ID=%u", pInfo->nExpireTime, pInfo->uiMailID);
	if (nStrLen <= 0 || nStrLen >= sizeof(m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}

	if (!Query(m_strSQL, nStrLen))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return;
	}
}

void CRoleDBThread::RequestUpdateLastResetTime(const void *pPack, const unsigned int uiPackLen)
{
	GS2CS_LAST_5_CLOCK_RESET_TIME	*pInfo	= (GS2CS_LAST_5_CLOCK_RESET_TIME*)pPack;

	int	nStrLen	= snprintf(m_strSQL, sizeof(m_strSQL), "update gameserver set Last5ClockResetTime=%d where ID=%hu", pInfo->nLastTime, pInfo->wGameServerID);
	if (nStrLen <= 0 || nStrLen >= sizeof(m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}

	if (!Query(m_strSQL, nStrLen))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return;
	}
}

void CRoleDBThread::RecvLastPerDayResetTime(const void *pPack, const unsigned int uiPackLen)
{
	GS2CS_LAST_PER_DAY_RESET_TIME	*pInfo	= (GS2CS_LAST_PER_DAY_RESET_TIME*)pPack;

	int	nStrLen	= snprintf(m_strSQL, sizeof(m_strSQL), "update gameserver set LastPerDayResetTime=%d where ID=%hu", pInfo->nLastTime, pInfo->wGameServerID);
	if (nStrLen <= 0 || nStrLen >= sizeof(m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}

	if (!Query(m_strSQL, nStrLen))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return;
	}
}

void CRoleDBThread::RecvQueryCommonRemain(const void *pPack, const unsigned int uiPackLen)
{
	ROLE_DB_QUERY_COMMON_REMAIN	*pInfo	= (ROLE_DB_QUERY_COMMON_REMAIN*)pPack;

	int	nStrLen	= snprintf(m_strSQL, sizeof(m_strSQL), "call QueryCommonRemain(%d)", pInfo->nDateTime);
	if (nStrLen <= 0 || nStrLen >= sizeof(m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}

	if (!Query(m_strSQL, nStrLen))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return;
	}

	m_pQueryRes = mysql_store_result(m_pDBHandle);
	if (NULL == m_pQueryRes)
	{
		g_pFileLog->WriteLog("%s[%d] mysql_store_result Failed\n", __FILE__, __LINE__);
		return;
	}

	if (1 != mysql_num_rows(m_pQueryRes))
	{
		g_pFileLog->WriteLog("[%s][%d] Result Data Error\n", __FILE__, __LINE__);
		mysql_free_result(m_pQueryRes);
		return;
	}

	m_pRow = mysql_fetch_row(m_pQueryRes);
	if (NULL == m_pRow)
	{
		g_pFileLog->WriteLog("%s[%d] mysql_fetch_row Failed\n", __FILE__, __LINE__);
		mysql_free_result(m_pQueryRes);
		return;
	}

	unsigned long	*pRowLength	= mysql_fetch_lengths(m_pQueryRes);
	if (!pRowLength)
	{
		g_pFileLog->WriteLog("%s[%d] mysql_fetch_lengths Failed\n", __FILE__, __LINE__);
		mysql_free_result(m_pQueryRes);
		return;
	}

	int	nColIndex	= 0;

	RDB_RESPOND_QUERY_COMMON_REMAIN	tagInfo;
	memset(&tagInfo, 0, sizeof(tagInfo));

	tagInfo.uDau				= (UINT)strtoul(m_pRow[nColIndex], NULL, 10);

	++nColIndex;
	tagInfo.uDnu				= (UINT)strtoul(m_pRow[nColIndex], NULL, 10);

	++nColIndex;
	tagInfo.uSecondRemain		= (UINT)strtoul(m_pRow[nColIndex], NULL, 10);

	++nColIndex;
	tagInfo.uThirdRemain		= (UINT)strtoul(m_pRow[nColIndex], NULL, 10);

	++nColIndex;
	tagInfo.uSeventhRemain		= (UINT)strtoul(m_pRow[nColIndex], NULL, 10);

	++nColIndex;
	tagInfo.uThirtiethRemain	= (UINT)strtoul(m_pRow[nColIndex], NULL, 10);

	tagInfo.byProtocol	= rdb_respond_query_common_remain;
	tagInfo.wServerID	= pInfo->wServerID;
	strncpy(tagInfo.strGMAccount, pInfo->strAccount, sizeof(tagInfo.strGMAccount));
	tagInfo.strGMAccount[sizeof(tagInfo.strGMAccount)-1]	= '\0';

	m_pRBRoleDBRespond->SndPack(&tagInfo, sizeof(tagInfo));

	mysql_free_result(m_pQueryRes);

	while (!mysql_next_result(m_pDBHandle))
	{
		MYSQL_RES	*pResult = mysql_store_result(m_pDBHandle);
		mysql_free_result(pResult);
	}
}

void CRoleDBThread::RecvCreateGuild(const void *pPack, const unsigned int uiPackLen)
{
	GS2CS_CREATE_GUILD	*pInfo	= (GS2CS_CREATE_GUILD*)pPack;

	char			*pszPos		= m_strSQL;
	int				nStrLen		= 0;
	unsigned int	uLeftSize	= sizeof(m_strSQL);
	int				nLength		= 0;

	pInfo->strGuildName[sizeof(pInfo->strGuildName)-1]	= '\0';
	pInfo->strAnnouncement[sizeof(pInfo->strAnnouncement)-1]	= '\0';

	nStrLen = snprintf(pszPos, uLeftSize, "call CreateGuild(%u,'", pInfo->uPlayerID);
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}

	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= strlen(pInfo->strGuildName);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen = mysql_real_escape_string(m_pDBHandle, pszPos, pInfo->strGuildName, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_real_escape_string SQL Error\n", __FILE__, __LINE__);
			return;
		}

		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen = snprintf(pszPos, uLeftSize, "','");
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}

	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= strlen(pInfo->strAnnouncement);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen = mysql_real_escape_string(m_pDBHandle, pszPos, pInfo->strAnnouncement, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_real_escape_string SQL Error\n", __FILE__, __LINE__);
			return;
		}

		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen = snprintf(pszPos, uLeftSize, "',%hhu,%u,%hhu)", pInfo->byMaxMemberCount, pInfo->uJoinPoint, pInfo->byJoinType);
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}

	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	if (!Query(m_strSQL, pszPos-m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return;
	}

	m_pQueryRes = mysql_store_result(m_pDBHandle);
	if (NULL == m_pQueryRes)
	{
		g_pFileLog->WriteLog("%s[%d] mysql_store_result Failed\n", __FILE__, __LINE__);
		return;
	}

	m_pRow = mysql_fetch_row(m_pQueryRes);
	if (NULL == m_pRow)
	{
		g_pFileLog->WriteLog("%s[%d] mysql_fetch_row Failed\n", __FILE__, __LINE__);

		mysql_free_result(m_pQueryRes);

		while (!mysql_next_result(m_pDBHandle))
		{
			MYSQL_RES	*pResult = mysql_store_result(m_pDBHandle);
			mysql_free_result(pResult);
		}

		return;
	}

	unsigned long	*pRowLength	= mysql_fetch_lengths(m_pQueryRes);
	if (!pRowLength)
	{
		g_pFileLog->WriteLog("%s[%d] mysql_fetch_lengths Failed\n", __FILE__, __LINE__);

		mysql_free_result(m_pQueryRes);

		while (!mysql_next_result(m_pDBHandle))
		{
			MYSQL_RES	*pResult = mysql_store_result(m_pDBHandle);
			mysql_free_result(pResult);
		}

		return;
	}

	int		nColIndex	= 0;
	BYTE	byResult	= (BYTE)strtoul(m_pRow[nColIndex++], NULL, 10);

	if (byResult)
	{
		RDB_CREATE_GUILD_FAILED	tagFailed;
		tagFailed.byProtocol	= rdb_create_guild_failed;
		tagFailed.byResult		= byResult;
		tagFailed.uPlayerID		= pInfo->uPlayerID;
		tagFailed.wClientIndex	= pInfo->wClientIndex;
		tagFailed.wGameServerID	= pInfo->wGameServerID;
		tagFailed.wGSNetwork	= pInfo->wGSNetwork;
		m_pRBRoleDBRespond->SndPack(&tagFailed, sizeof(tagFailed));

		mysql_free_result(m_pQueryRes);

		while (!mysql_next_result(m_pDBHandle))
		{
			MYSQL_RES	*pResult = mysql_store_result(m_pDBHandle);
			mysql_free_result(pResult);
		}

		return;
	}

	RDB_CREATE_GUILD_SUCCESS	tagRespond;
	memset(&tagRespond, 0, sizeof(tagRespond));

	tagRespond.byProtocol		= rdb_create_guild_success;
	tagRespond.uPlayerID		= pInfo->uPlayerID;
	tagRespond.wClientIndex		= pInfo->wClientIndex;
	tagRespond.wGameServerID	= pInfo->wGameServerID;
	tagRespond.wGSNetwork		= pInfo->wGSNetwork;
	tagRespond.wGuildID			= (WORD)strtoul(m_pRow[nColIndex++], NULL, 10);
	tagRespond.byJoinType		= pInfo->byJoinType;
	tagRespond.byMaxMemberCount	= pInfo->byMaxMemberCount;
	tagRespond.uJoinPoint		= pInfo->uJoinPoint;
	strncpy(tagRespond.strGuildName, pInfo->strGuildName, sizeof(tagRespond.strGuildName));
	tagRespond.strGuildName[sizeof(tagRespond.strGuildName)-1]	= '\0';
	strncpy(tagRespond.strAnnouncement, pInfo->strAnnouncement, sizeof(tagRespond.strAnnouncement));
	tagRespond.strAnnouncement[sizeof(tagRespond.strAnnouncement)-1]	= '\0';

	m_pRBRoleDBRespond->SndPack(&tagRespond, sizeof(tagRespond));

	mysql_free_result(m_pQueryRes);

	while (!mysql_next_result(m_pDBHandle))
	{
		MYSQL_RES	*pResult = mysql_store_result(m_pDBHandle);
		mysql_free_result(pResult);
	}
}

void CRoleDBThread::RecvApplyJoinGuild(const void *pPack, const unsigned int uiPackLen)
{
	GS2CS_APPLY_JOIN_GUILD	*pInfo	= (GS2CS_APPLY_JOIN_GUILD*)pPack;

	char			*pszPos		= m_strSQL;
	int				nStrLen		= 0;
	unsigned int	uLeftSize	= sizeof(m_strSQL);
	int				nLength		= 0;

	nStrLen = snprintf(pszPos, uLeftSize, "call ApplyJoinGuild(%u,%hu,%hu)", pInfo->uPlayerID, pInfo->wGuildID, pInfo->wMaxApplyCount);
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}

	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	if (!Query(m_strSQL, pszPos-m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return;
	}

	m_pQueryRes = mysql_store_result(m_pDBHandle);
	if (NULL == m_pQueryRes)
	{
		g_pFileLog->WriteLog("%s[%d] mysql_store_result Failed\n", __FILE__, __LINE__);
		return;
	}

	m_pRow = mysql_fetch_row(m_pQueryRes);
	if (NULL == m_pRow)
	{
		g_pFileLog->WriteLog("%s[%d] mysql_fetch_row Failed\n", __FILE__, __LINE__);

		mysql_free_result(m_pQueryRes);

		while (!mysql_next_result(m_pDBHandle))
		{
			MYSQL_RES	*pResult = mysql_store_result(m_pDBHandle);
			mysql_free_result(pResult);
		}

		return;
	}

	unsigned long	*pRowLength	= mysql_fetch_lengths(m_pQueryRes);
	if (!pRowLength)
	{
		g_pFileLog->WriteLog("%s[%d] mysql_fetch_lengths Failed\n", __FILE__, __LINE__);

		mysql_free_result(m_pQueryRes);

		while (!mysql_next_result(m_pDBHandle))
		{
			MYSQL_RES	*pResult = mysql_store_result(m_pDBHandle);
			mysql_free_result(pResult);
		}

		return;
	}

	int		nColIndex	= 0;
	RDB_APPLY_JOIN_GUILD_RESULT	tagInfo;
	tagInfo.byProtocol		= rdb_apply_join_guild_result;
	tagInfo.uPlayerID		= pInfo->uPlayerID;
	tagInfo.wClientIndex	= pInfo->wClientIndex;
	tagInfo.wGameServerID	= pInfo->wGameServerID;
	tagInfo.wGSNetwork		= pInfo->wGSNetwork;
	tagInfo.wGuildID		= pInfo->wGuildID;
	tagInfo.byResult		= (BYTE)strtoul(m_pRow[nColIndex++], NULL, 10);

	m_pRBRoleDBRespond->SndPack(&tagInfo, sizeof(tagInfo));

	mysql_free_result(m_pQueryRes);

	while (!mysql_next_result(m_pDBHandle))
	{
		MYSQL_RES	*pResult = mysql_store_result(m_pDBHandle);
		mysql_free_result(pResult);
	}
}

void CRoleDBThread::RecvRefuseJoinGuild(const void *pPack, const unsigned int uiPackLen)
{
	GS2CS_REFUSE_JOIN_GUILD	*pInfo	= (GS2CS_REFUSE_JOIN_GUILD*)pPack;

	char			*pszPos		= m_strSQL;
	int				nStrLen		= 0;
	unsigned int	uLeftSize	= sizeof(m_strSQL);
	int				nLength		= 0;

	nStrLen = snprintf(pszPos, uLeftSize, "call RefuseJoinGuild(%u,%hu)", pInfo->uTargetID, pInfo->wGuildID);
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}

	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	if (!Query(m_strSQL, pszPos-m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return;
	}

	m_pQueryRes = mysql_store_result(m_pDBHandle);
	if (NULL == m_pQueryRes)
	{
		g_pFileLog->WriteLog("%s[%d] mysql_store_result Failed\n", __FILE__, __LINE__);
		return;
	}

	m_pRow = mysql_fetch_row(m_pQueryRes);
	if (NULL == m_pRow)
	{
		g_pFileLog->WriteLog("%s[%d] mysql_fetch_row Failed\n", __FILE__, __LINE__);

		mysql_free_result(m_pQueryRes);

		while (!mysql_next_result(m_pDBHandle))
		{
			MYSQL_RES	*pResult = mysql_store_result(m_pDBHandle);
			mysql_free_result(pResult);
		}

		return;
	}

	unsigned long	*pRowLength	= mysql_fetch_lengths(m_pQueryRes);
	if (!pRowLength)
	{
		g_pFileLog->WriteLog("%s[%d] mysql_fetch_lengths Failed\n", __FILE__, __LINE__);

		mysql_free_result(m_pQueryRes);

		while (!mysql_next_result(m_pDBHandle))
		{
			MYSQL_RES	*pResult = mysql_store_result(m_pDBHandle);
			mysql_free_result(pResult);
		}

		return;
	}

	int		nColIndex	= 0;
	RDB_REFUSE_JOIN_GUILD_RESULT	tagInfo;
	tagInfo.byProtocol		= rdb_refuse_join_guild_result;
	tagInfo.uPlayerID		= pInfo->uPlayerID;
	tagInfo.uTargetID		= pInfo->uTargetID;
	tagInfo.wClientIndex	= pInfo->wClientIndex;
	tagInfo.wGameServerID	= pInfo->wGameServerID;
	tagInfo.wGSNetwork		= pInfo->wGSNetwork;
	tagInfo.wGuildID		= pInfo->wGuildID;
	tagInfo.byResult		= (BYTE)strtoul(m_pRow[nColIndex++], NULL, 10);

	m_pRBRoleDBRespond->SndPack(&tagInfo, sizeof(tagInfo));

	mysql_free_result(m_pQueryRes);

	while (!mysql_next_result(m_pDBHandle))
	{
		MYSQL_RES	*pResult = mysql_store_result(m_pDBHandle);
		mysql_free_result(pResult);
	}
}

void CRoleDBThread::RecvApproveJoinGuild(const void *pPack, const unsigned int uiPackLen)
{
	GS2CS_APPROVE_JOIN_GUILD	*pInfo	= (GS2CS_APPROVE_JOIN_GUILD*)pPack;

	char			*pszPos		= m_strSQL;
	int				nStrLen		= 0;
	unsigned int	uLeftSize	= sizeof(m_strSQL);
	int				nLength		= 0;

	nStrLen = snprintf(pszPos, uLeftSize, "call ApproveJoinGuild(%u,%hu)", pInfo->uTargetID, pInfo->wGuildID);
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}

	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	if (!Query(m_strSQL, pszPos-m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return;
	}

	m_pQueryRes = mysql_store_result(m_pDBHandle);
	if (NULL == m_pQueryRes)
	{
		g_pFileLog->WriteLog("%s[%d] mysql_store_result Failed\n", __FILE__, __LINE__);
		return;
	}

	m_pRow = mysql_fetch_row(m_pQueryRes);
	if (NULL == m_pRow)
	{
		g_pFileLog->WriteLog("%s[%d] mysql_fetch_row Failed\n", __FILE__, __LINE__);

		mysql_free_result(m_pQueryRes);

		while (!mysql_next_result(m_pDBHandle))
		{
			MYSQL_RES	*pResult = mysql_store_result(m_pDBHandle);
			mysql_free_result(pResult);
		}

		return;
	}

	unsigned long	*pRowLength	= mysql_fetch_lengths(m_pQueryRes);
	if (!pRowLength)
	{
		g_pFileLog->WriteLog("%s[%d] mysql_fetch_lengths Failed\n", __FILE__, __LINE__);

		mysql_free_result(m_pQueryRes);

		while (!mysql_next_result(m_pDBHandle))
		{
			MYSQL_RES	*pResult = mysql_store_result(m_pDBHandle);
			mysql_free_result(pResult);
		}

		return;
	}

	int		nColIndex	= 0;
	RDB_APPROVE_JOIN_GUILD_RESULT	tagInfo;
	tagInfo.byProtocol		= rdb_approve_join_guild_result;
	tagInfo.uPlayerID		= pInfo->uPlayerID;
	tagInfo.uTargetID		= pInfo->uTargetID;
	tagInfo.wClientIndex	= pInfo->wClientIndex;
	tagInfo.wGameServerID	= pInfo->wGameServerID;
	tagInfo.wGSNetwork		= pInfo->wGSNetwork;
	tagInfo.wGuildID		= pInfo->wGuildID;
	tagInfo.byResult		= (BYTE)strtoul(m_pRow[nColIndex++], NULL, 10);

	m_pRBRoleDBRespond->SndPack(&tagInfo, sizeof(tagInfo));

	mysql_free_result(m_pQueryRes);

	while (!mysql_next_result(m_pDBHandle))
	{
		MYSQL_RES	*pResult = mysql_store_result(m_pDBHandle);
		mysql_free_result(pResult);
	}
}

void CRoleDBThread::RecvPlayerJoinGuild(const void *pPack, const unsigned int uiPackLen)
{
	GS2CS_PLAYER_JOIN_GUILD	*pInfo	= (GS2CS_PLAYER_JOIN_GUILD*)pPack;

	char			*pszPos		= m_strSQL;
	int				nStrLen		= 0;
	unsigned int	uLeftSize	= sizeof(m_strSQL);
	int				nLength		= 0;

	nStrLen = snprintf(pszPos, uLeftSize, "call PlayerJoinGuild(%u,%hu)", pInfo->uPlayerID, pInfo->wGuildID);
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}

	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	if (!Query(m_strSQL, pszPos-m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return;
	}

	m_pQueryRes = mysql_store_result(m_pDBHandle);
	if (NULL == m_pQueryRes)
	{
		g_pFileLog->WriteLog("%s[%d] mysql_store_result Failed\n", __FILE__, __LINE__);
		return;
	}

	m_pRow = mysql_fetch_row(m_pQueryRes);
	if (NULL == m_pRow)
	{
		g_pFileLog->WriteLog("%s[%d] mysql_fetch_row Failed\n", __FILE__, __LINE__);

		mysql_free_result(m_pQueryRes);

		while (!mysql_next_result(m_pDBHandle))
		{
			MYSQL_RES	*pResult = mysql_store_result(m_pDBHandle);
			mysql_free_result(pResult);
		}

		return;
	}

	unsigned long	*pRowLength	= mysql_fetch_lengths(m_pQueryRes);
	if (!pRowLength)
	{
		g_pFileLog->WriteLog("%s[%d] mysql_fetch_lengths Failed\n", __FILE__, __LINE__);

		mysql_free_result(m_pQueryRes);

		while (!mysql_next_result(m_pDBHandle))
		{
			MYSQL_RES	*pResult = mysql_store_result(m_pDBHandle);
			mysql_free_result(pResult);
		}

		return;
	}

	int		nColIndex	= 0;
	RDB_PLAYER_JOIN_GUILD_RESULT	tagInfo;
	tagInfo.byProtocol		= rdb_approve_join_guild_result;
	tagInfo.uPlayerID		= pInfo->uPlayerID;
	tagInfo.wClientIndex	= pInfo->wClientIndex;
	tagInfo.wGameServerID	= pInfo->wGameServerID;
	tagInfo.wGSNetwork		= pInfo->wGSNetwork;
	tagInfo.wGuildID		= pInfo->wGuildID;
	tagInfo.byResult		= (BYTE)strtoul(m_pRow[nColIndex++], NULL, 10);

	m_pRBRoleDBRespond->SndPack(&tagInfo, sizeof(tagInfo));

	mysql_free_result(m_pQueryRes);

	while (!mysql_next_result(m_pDBHandle))
	{
		MYSQL_RES	*pResult = mysql_store_result(m_pDBHandle);
		mysql_free_result(pResult);
	}
}

void CRoleDBThread::RecvUpdateCTRanking(const void *pPack, const unsigned int uiPackLen)
{
	GS2CS_UPDATE_CT_RANKING	*pInfo	= (GS2CS_UPDATE_CT_RANKING*)pPack;

	char			*pszPos		= m_strSQL;
	int				nStrLen		= 0;
	unsigned int	uLeftSize	= sizeof(m_strSQL);
	int				nLength		= 0;

	nStrLen = snprintf(pszPos, uLeftSize, "call PlayerClimbTower(%u,%hu,%hu,%u)", pInfo->uPlayerID, pInfo->wGameServerID, pInfo->wMissionID, pInfo->uRanking);
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}

	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	if (!Query(m_strSQL, pszPos-m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return;
	}
}

void CRoleDBThread::RecvLoadFriendLog(const void *pPack, const unsigned int uiPackLen)
{
	GS2CS_LOAD_FRIEND_LOG	*pInfo		= (GS2CS_LOAD_FRIEND_LOG*)pPack;

	char					*pszPos		= m_strSQL;
	int						nStrLen		= 0;
	unsigned int			uLeftSize	= sizeof(m_strSQL);
	int						nLength		= 0;

	nStrLen = snprintf(pszPos, uLeftSize, "call LoadFriendLog(%u)", pInfo->uPlayerID);
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}

	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	if (!Query(m_strSQL, pszPos-m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return;
	}

	char	strBuffer[sizeof(RDB_FRIEND_LOG)+MAX_FRIEND_LOG*sizeof(SDBFriendLog)];
	memset(strBuffer, 0, sizeof(strBuffer));

	RDB_FRIEND_LOG	*pSendInfo	= (RDB_FRIEND_LOG*)strBuffer;
	SDBFriendLog	*pFriendLog	= (SDBFriendLog*)(strBuffer+sizeof(RDB_FRIEND_LOG));

	m_pQueryRes = mysql_use_result(m_pDBHandle);
	if (!m_pQueryRes)
	{
		g_pFileLog->WriteLog("%s[%d] mysql_use_result Failed!\n", __FILE__, __LINE__);
		return;
	}

	int		nColIndex;
	BYTE	byCount	= 0;

	while (NULL != (m_pRow = mysql_fetch_row(m_pQueryRes)) && byCount < MAX_FRIEND_LOG)
	{
		nColIndex	= 0;
		pFriendLog->uFriendID	= (UINT)strtoul(m_pRow[nColIndex++], NULL, 10);

		pFriendLog->byOperation	= (BYTE)strtoul(m_pRow[nColIndex++], NULL, 10);

		++byCount;
		++pFriendLog;
	}

	pSendInfo->byProtocol		= rdb_friend_log;
	pSendInfo->uPlayerID		= pInfo->uPlayerID;
	pSendInfo->wClientIndex		= pInfo->wClientIndex;
	pSendInfo->wGameServerID	= pInfo->wGameServerID;
	pSendInfo->wGSNetwork		= pInfo->wGSNetwork;
	pSendInfo->wSize			= sizeof(RDB_FRIEND_LOG) - sizeof(SUndefinedProtocolHead) + sizeof(SDBFriendLog) * byCount;

	m_pRBRoleDBRespond->SndPack(strBuffer, pSendInfo->wSize + sizeof(SUndefinedProtocolHead));

	mysql_free_result(m_pQueryRes);

	while (!mysql_next_result(m_pDBHandle))
	{
		MYSQL_RES	*pResult = mysql_store_result(m_pDBHandle);
		mysql_free_result(pResult);
	}
}

void CRoleDBThread::RecvUpdateFriend(const void *pPack, const unsigned int uiPackLen)
{
	ROLE_DB_UPDATE_FRIEND	*pInfo		= (ROLE_DB_UPDATE_FRIEND*)pPack;

	char					*pszPos		= m_strSQL;
	int						nStrLen		= 0;
	unsigned int			uLeftSize	= sizeof(m_strSQL);
	int						nLength		= 0;

	nStrLen = snprintf(pszPos, uLeftSize, "call UpdateFriendData(%u,'", pInfo->uPlayerID);
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}

	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= sizeof(pInfo->tagFriend);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)(&pInfo->tagFriend), nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_real_escape_string SQL Error\n", __FILE__, __LINE__);
			return;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen = snprintf(pszPos, uLeftSize, "')");
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	if (!Query(m_strSQL, pszPos-m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return;
	}
}

void CRoleDBThread::RecvAddFriendLog(const void *pPack, const unsigned int uiPackLen)
{
	GS2CS_ADD_FRIEND_LOG	*pInfo		= (GS2CS_ADD_FRIEND_LOG*)pPack;

	char					*pszPos		= m_strSQL;
	int						nStrLen		= 0;
	unsigned int			uLeftSize	= sizeof(m_strSQL);
	int						nLength		= 0;

	nStrLen = snprintf(pszPos, uLeftSize, "call AddFriendLog(%u,%u,%hhu,%d)", pInfo->uPlayerID, pInfo->uFriendID, pInfo->byOperation, pInfo->nExpireTime);
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}

	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	if (!Query(m_strSQL, pszPos-m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return;
	}
}

void CRoleDBThread::RecvDelFriendLog(const void *pPack, const unsigned int uiPackLen)
{
	char			*pszPos		= m_strSQL;
	int				nStrLen		= 0;
	unsigned int	uLeftSize	= sizeof(m_strSQL);
	int				nLength		= 0;

	nStrLen = snprintf(pszPos, uLeftSize, "call DeleteFriendLog()");
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}

	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	if (!Query(m_strSQL, pszPos-m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return;
	}
}

void CRoleDBThread::RecvCreateRedEnvelope(const void *pPack, const unsigned int uiPackLen)
{
	GS2CS_CREATE_RED_ENVELOPE	*pInfo		= (GS2CS_CREATE_RED_ENVELOPE*)pPack;
	char						*pszPos		= m_strSQL;
	int							nStrLen		= 0;
	unsigned int				uLeftSize	= sizeof(m_strSQL);
	int							nLength		= 0;

	pInfo->strPlayerName[sizeof(pInfo->strPlayerName)-1]	= '\0';

	nStrLen = snprintf(pszPos, uLeftSize, "call CreateRedEnvelope(%u,%hu,%u,'", pInfo->uID, pInfo->wGameServerID, pInfo->uPlayerID);
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}

	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= strlen(pInfo->strPlayerName);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen = mysql_real_escape_string(m_pDBHandle, pszPos, pInfo->strPlayerName, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_real_escape_string SQL Error\n", __FILE__, __LINE__);
			return;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen = snprintf(pszPos, uLeftSize, "',%hhu,%u,%u,%d)", pInfo->byImageID, pInfo->uMinDiamond, pInfo->uMaxDiamond, pInfo->nExpireTime);
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	if (!Query(m_strSQL, pszPos-m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return;
	}
}

void CRoleDBThread::RecvUpdateRedEnvelope(const void *pPack, const unsigned int uiPackLen)
{
	GS2CS_UPDATE_RED_ENVELOPE	*pInfo		= (GS2CS_UPDATE_RED_ENVELOPE*)pPack;
	char						*pszPos		= m_strSQL;
	int							nStrLen		= 0;
	unsigned int				uLeftSize	= sizeof(m_strSQL);
	int							nLength		= 0;

	nStrLen = snprintf(pszPos, uLeftSize, "call UpdateRedEnvelope(%u,%u,'", pInfo->uID, pInfo->wGameServerID);
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}

	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= sizeof(pInfo->tagData);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)(&pInfo->tagData), nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_real_escape_string SQL Error\n", __FILE__, __LINE__);
			return;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen = snprintf(pszPos, uLeftSize, "')");
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	if (!Query(m_strSQL, pszPos-m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return;
	}
}

void CRoleDBThread::RecvDeleteRedEnvelope(const void *pPack, const unsigned int uiPackLen)
{
	GS2CS_DELETE_RED_ENVELOPE	*pInfo		= (GS2CS_DELETE_RED_ENVELOPE*)pPack;
	char						*pszPos		= m_strSQL;
	int							nStrLen		= 0;
	unsigned int				uLeftSize	= sizeof(m_strSQL);
	int							nLength		= 0;

	nStrLen = snprintf(pszPos, uLeftSize, "call DeleteRedEnvelope(%u,%u)", pInfo->uID, pInfo->wGameServerID);
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}

	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	if (!Query(m_strSQL, pszPos-m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return;
	}
}

void CRoleDBThread::RecvAddFriendWhisper(const void *pPack, const unsigned int uiPackLen)
{
	GS2CS_ADD_FRIEND_WHISPER	*pInfo			= (GS2CS_ADD_FRIEND_WHISPER*)pPack;
	char						*pszPos			= m_strSQL;
	int							nStrLen			= 0;
	unsigned int				uLeftSize		= sizeof(m_strSQL);
	int							nLength			= 0;

	pInfo->strWhisper[MAX_CHAT_MESSAGE_SIZE-1]	= '\0';

	nStrLen = snprintf(pszPos, uLeftSize, "call AddFriendWhisper(%u,%u,%d,'", pInfo->uSenderID, pInfo->uReceiverID, pInfo->nExpireTime);
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}

	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= strlen(pInfo->strWhisper);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen = mysql_real_escape_string(m_pDBHandle, pszPos, pInfo->strWhisper, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_real_escape_string SQL Error\n", __FILE__, __LINE__);
			return;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen = snprintf(pszPos, uLeftSize, "')");
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	if (!Query(m_strSQL, pszPos-m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return;
	}
}

void CRoleDBThread::RecvRequestFriendWhisper(const void *pPack, const unsigned int uiPackLen)
{
	GS2CS_REQUEST_FRIEND_WHISPER	*pInfo		= (GS2CS_REQUEST_FRIEND_WHISPER*)pPack;
	char							*pszPos		= m_strSQL;
	int								nStrLen		= 0;
	unsigned int					uLeftSize	= sizeof(m_strSQL);
	int								nLength		= 0;

	nStrLen = snprintf(pszPos, uLeftSize, "call LoadFriendWhisper(%u,%u)", pInfo->uPlayerID, pInfo->uFriendID);
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}

	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	if (!Query(m_strSQL, pszPos-m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return;
	}

	char	strBuffer[sizeof(RDB_FRIEND_WHISPER)+MAX_FRIEND_WHISPER_COUNT*sizeof(SDBFriendWhisper)];
	memset(strBuffer, 0, sizeof(strBuffer));

	RDB_FRIEND_WHISPER	*pSendInfo	= (RDB_FRIEND_WHISPER*)strBuffer;
	SDBFriendWhisper	*pWhisper	= (SDBFriendWhisper*)(strBuffer+sizeof(RDB_FRIEND_WHISPER));

	m_pQueryRes = mysql_use_result(m_pDBHandle);
	if (!m_pQueryRes)
	{
		g_pFileLog->WriteLog("%s[%d] mysql_use_result Failed!\n", __FILE__, __LINE__);
		return;
	}

	int		nColIndex;
	BYTE	byCount	= 0;

	while (NULL != (m_pRow = mysql_fetch_row(m_pQueryRes)) && byCount < MAX_FRIEND_WHISPER_COUNT)
	{
		nColIndex	= 0;
		pWhisper->uSenderID	= (UINT)strtoul(m_pRow[nColIndex++], NULL, 10);

		strncpy(pWhisper->strWhisper, m_pRow[nColIndex++], sizeof(pWhisper->strWhisper));
		pWhisper->strWhisper[sizeof(pWhisper->strWhisper)-1]	= '\0';

		++byCount;
		++pWhisper;
	}

	pSendInfo->byProtocol		= rdb_friend_whisper;
	pSendInfo->uPlayerID		= pInfo->uPlayerID;
	pSendInfo->uFriendID		= pInfo->uFriendID;
	pSendInfo->wClientIndex		= pInfo->wClientIndex;
	pSendInfo->wGameServerID	= pInfo->wGameServerID;
	pSendInfo->wSize			= sizeof(RDB_FRIEND_WHISPER) - sizeof(SUndefinedProtocolHead) + sizeof(SDBFriendWhisper) * byCount;

	m_pRBRoleDBRespond->SndPack(strBuffer, pSendInfo->wSize + sizeof(SUndefinedProtocolHead));

	mysql_free_result(m_pQueryRes);

	while (!mysql_next_result(m_pDBHandle))
	{
		MYSQL_RES	*pResult = mysql_store_result(m_pDBHandle);
		mysql_free_result(pResult);
	}
}

void CRoleDBThread::RequestUpdateLastPVPAwardTime(const void *pPack, const unsigned int uiPackLen)
{
	GS2CS_LAST_PVP_AWARD_TIME	*pInfo	= (GS2CS_LAST_PVP_AWARD_TIME*)pPack;

	int	nStrLen	= snprintf(m_strSQL, sizeof(m_strSQL), "update gameserver set LastPVPAwardTime=%d where ID=%hu", pInfo->nLastPVPAwardTime, pInfo->wGameServerID);
	if (nStrLen <= 0 || nStrLen >= sizeof(m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}

	if (!Query(m_strSQL, nStrLen))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return;
	}
}

void CRoleDBThread::RequestUpdateRoleTDRanking(const void *pPack, const unsigned int uiPackLen)
{
	ROLE_DB_UPDATE_TD_RANKING	*pInfo			= (ROLE_DB_UPDATE_TD_RANKING*)pPack;
	char						*pszPos			= m_strSQL;
	my_ulonglong				lAffectedRow	= 0;
	int							nStrLen			= 0;
	unsigned int				uLeftSize		= sizeof(m_strSQL);
	int							nLength			= 0;

	nStrLen	= snprintf(m_strSQL, sizeof(m_strSQL), "update role set TowerDefence='");
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= sizeof(pInfo->tagTowerDefence);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)(&pInfo->tagTowerDefence), nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] mysql_real_escape_string SQL Error\n", __FILE__, __LINE__);
			return;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen = snprintf(pszPos, uLeftSize, "' where PlayerID=%u", pInfo->uPlayerID);
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	if (!Query(m_strSQL, pszPos-m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return;
	}

	lAffectedRow	= mysql_affected_rows(m_pDBHandle);
	if (lAffectedRow > 1)
	{
		g_pFileLog->WriteLog("%s[%d] Save Role Failed:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return;
	}
}

void CRoleDBThread::RequestUpdateOnlinePVPPoint(const void *pPack, const unsigned int uiPackLen)
{
	ROLE_DB_UPDATE_ONLINE_PVP_POINT	*pInfo	= (ROLE_DB_UPDATE_ONLINE_PVP_POINT*)pPack;

	int	nStrLen	= snprintf(m_strSQL, sizeof(m_strSQL), "update role set OnlinePVPPoint=%u,OnlinePVPFightCount=%u,OnlinePVPWinCount=%u where PlayerID=%hu", pInfo->uPoint, pInfo->uFightCount, pInfo->uWinCount, pInfo->uPlayerID);
	if (nStrLen <= 0 || nStrLen >= sizeof(m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}

	if (!Query(m_strSQL, nStrLen))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return;
	}
}

void CRoleDBThread::RequestLastOnlinePVPAwardTime(const void *pPack, const unsigned int uiPackLen)
{
	ROLE_DB_ONLINE_PVP_AWARD_TIME	*pInfo	= (ROLE_DB_ONLINE_PVP_AWARD_TIME*)pPack;

	int	nStrLen	= snprintf(m_strSQL, sizeof(m_strSQL), "update activityserver set LastPVPAwardTime=%d where ID=%hu", pInfo->nLastAwardTime, pInfo->wActivityID);
	if (nStrLen <= 0 || nStrLen >= sizeof(m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}

	if (!Query(m_strSQL, nStrLen))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return;
	}
}

void CRoleDBThread::RequestUpdateOnlinePVPTitle(const void *pPack, const unsigned int uiPackLen)
{
	ROLE_DB_UPDATE_ONLINE_PVP_TITLE	*pInfo	= (ROLE_DB_UPDATE_ONLINE_PVP_TITLE*)pPack;

	int	nStrLen	= snprintf(m_strSQL, sizeof(m_strSQL), "update role set OnlinePVPLastTitle=%hhu where PlayerID=%hu", pInfo->byTitle, pInfo->uPlayerID);
	if (nStrLen <= 0 || nStrLen >= sizeof(m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}

	if (!Query(m_strSQL, nStrLen))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return;
	}
}

void CRoleDBThread::RequestAddRoleLoginRecord(const void *pPack, const unsigned int uiPackLen)
{
	ROLE_DB_ADD_ROLE_LOGIN_RECORD	*pInfo	= (ROLE_DB_ADD_ROLE_LOGIN_RECORD*)pPack;
	char				*pszPos		= m_strSQL;
	unsigned int		uLeftSize	= sizeof(m_strSQL);
	int					nLength		= 0;

	int	nStrLen	= snprintf(m_strSQL, sizeof(m_strSQL), "insert into rolelogin (PlayerID,ServerID,LoginTime,CreateTime) values(%u,%hu,now(),from_unixtime(%d))", pInfo->uPlayerID, pInfo->wServerID, pInfo->nCreateTime);
	if (nStrLen <= 0 || nStrLen >= sizeof(m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}

	if (!Query(m_strSQL, nStrLen))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return;
	}

	if (1 != mysql_affected_rows(m_pDBHandle))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return;
	}
}

void CRoleDBThread::RecvGMLog(const void *pPack, const unsigned int uiPackLen)
{
	ROLE_DB_ADD_GM_LOG	*pInfo		= (ROLE_DB_ADD_GM_LOG*)pPack;
	char				*pszPos		= m_strSQL;
	unsigned int		uLeftSize	= sizeof(m_strSQL);
	int					nLength		= 0;

	int	nStrLen	= snprintf(m_strSQL, sizeof(m_strSQL), "insert into gmlog (Account,IP,Operation,Param1,Param2,Param3,String1,String2,String3) values('");
	if (nStrLen <= 0 || nStrLen >= sizeof(m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}

	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= strlen(pInfo->strAccount);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen	= mysql_real_escape_string(m_pDBHandle, pszPos, pInfo->strAccount, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
			return;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen	= snprintf(pszPos, uLeftSize, "','");
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= sizeof(pInfo->strIP);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen	= mysql_real_escape_string(m_pDBHandle, pszPos, pInfo->strIP, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
			return;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen	= snprintf(pszPos, uLeftSize, "','");
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= sizeof(pInfo->strOperation);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen	= mysql_real_escape_string(m_pDBHandle, pszPos, pInfo->strOperation, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
			return;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen	= snprintf(pszPos, uLeftSize, "',%d,%d,%d,'", pInfo->nParam1, pInfo->nParam2, pInfo->nParam3);
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= sizeof(pInfo->strParam1);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen	= mysql_real_escape_string(m_pDBHandle, pszPos, pInfo->strParam1, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
			return;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen	= snprintf(pszPos, uLeftSize, "','");
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= sizeof(pInfo->strParam2);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen	= mysql_real_escape_string(m_pDBHandle, pszPos, pInfo->strParam2, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
			return;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen	= snprintf(pszPos, uLeftSize, "','");
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= sizeof(pInfo->strParam3);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen	= mysql_real_escape_string(m_pDBHandle, pszPos, pInfo->strParam3, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
			return;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen	= snprintf(pszPos, uLeftSize, "')");
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	if (!Query(m_strSQL, pszPos-m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return;
	}

	if (1 != mysql_affected_rows(m_pDBHandle))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return;
	}
}

void CRoleDBThread::RecvQueryRemain(const void *pPack, const unsigned int uiPackLen)
{
}

void CRoleDBThread::RecvAddGiftCode(const void *pPack, const unsigned int uiPackLen)
{
	GM2CS_ADD_GIFT_CODE	*pInfo		= (GM2CS_ADD_GIFT_CODE*)pPack;
	SGiftCode			*pGift		= (SGiftCode*)(((char*)pPack)+sizeof(GM2CS_ADD_GIFT_CODE));
	WORD				wGiftCount	= (uiPackLen - sizeof(GM2CS_ADD_GIFT_CODE)) / sizeof(SGiftCode);
	WORD				wAddCount	= 0;

	for (int nIndex = 0; nIndex < wGiftCount; ++nIndex, ++pGift)
	{
		if (AddGiftCode(pGift))
			++wAddCount;
	}

	RDB_RESPOND_ADD_GIFT_CODE	tagInfo;
	memset(&tagInfo, 0, sizeof(tagInfo));
	tagInfo.byProtocol	= rdb_respond_add_gift_code;
	tagInfo.wTotalCount	= wGiftCount;
	tagInfo.wAddCount	= wAddCount;
	strncpy(tagInfo.strGMAccount, pInfo->strGMAccount, sizeof(tagInfo.strGMAccount));
	tagInfo.strGMAccount[sizeof(tagInfo.strGMAccount)-1]	= '\0';

	m_pRBRoleDBRespond->SndPack(&tagInfo, sizeof(tagInfo));
}

void CRoleDBThread::RecvDelGiftCode(const void *pPack, const unsigned int uiPackLen)
{
	ROLE_DB_DEL_GIFT_CODE	*pInfo		= (ROLE_DB_DEL_GIFT_CODE*)pPack;
	char					*pszPos		= m_strSQL;
	unsigned int			uLeftSize	= sizeof(m_strSQL);
	int						nLength		= 0;

	int	nStrLen	= snprintf(m_strSQL, sizeof(m_strSQL), "delete from giftcode where GiftCodeID='");
	if (nStrLen <= 0 || nStrLen >= sizeof(m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}

	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= strlen(pInfo->strGiftCode);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen	= mysql_real_escape_string(m_pDBHandle, pszPos, pInfo->strGiftCode, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
			return;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen	= snprintf(pszPos, uLeftSize, "'");
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	if (!Query(m_strSQL, pszPos-m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return;
	}

	if (1 != mysql_affected_rows(m_pDBHandle))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return;
	}
}

void CRoleDBThread::RecvUpdateGiftCode(const void *pPack, const unsigned int uiPackLen)
{
}

void CRoleDBThread::RecvExchangeGiftCode(const void *pPack, const unsigned int uiPackLen)
{
	GS2CS_EXCHANGE_GIFT	*pInfo		= (GS2CS_EXCHANGE_GIFT*)pPack;
	char				*pszPos		= m_strSQL;
	unsigned int		uLeftSize	= sizeof(m_strSQL);
	int					nLength		= 0;

	int	nStrLen	= snprintf(m_strSQL, sizeof(m_strSQL), "call ExchangeGiftPack('");
	if (nStrLen <= 0 || nStrLen >= sizeof(m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}

	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= strlen(pInfo->strGiftCode);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen	= mysql_real_escape_string(m_pDBHandle, pszPos, pInfo->strGiftCode, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
			return;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen	= snprintf(pszPos, uLeftSize, "','");
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	nLength	= strlen(pInfo->strAccount);
	if (nLength > 0)
	{
		if (uLeftSize <= nLength << 1)
		{
			g_pFileLog->WriteLog("%s[%d] Buffer Not Enough\n", __FILE__, __LINE__);
			return;
		}
		nStrLen	= mysql_real_escape_string(m_pDBHandle, pszPos, pInfo->strAccount, nLength);
		if (nStrLen <= 0 || nStrLen >= uLeftSize)
		{
			g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
			return;
		}
		pszPos		+= nStrLen;
		uLeftSize	-= nStrLen;
	}

	nStrLen	= snprintf(pszPos, uLeftSize, "')");
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}
	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	if (!Query(m_strSQL, pszPos-m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]\n", __FILE__, __LINE__, m_strSQL);
		return;
	}

	m_pQueryRes = mysql_store_result(m_pDBHandle);
	if (NULL == m_pQueryRes)
	{
		g_pFileLog->WriteLog("%s[%d] mysql_store_result Failed\n", __FILE__, __LINE__);
		return;
	}

	if (1 != mysql_num_rows(m_pQueryRes))
	{
		g_pFileLog->WriteLog("[%s][%d] Result Data Error\n", __FILE__, __LINE__);
		mysql_free_result(m_pQueryRes);
		return;
	}

	m_pRow = mysql_fetch_row(m_pQueryRes);
	if (NULL == m_pRow)
	{
		g_pFileLog->WriteLog("%s[%d] mysql_fetch_row Failed\n", __FILE__, __LINE__);
		mysql_free_result(m_pQueryRes);
		return;
	}

	unsigned long	*pRowLength	= mysql_fetch_lengths(m_pQueryRes);
	if (!pRowLength)
	{
		g_pFileLog->WriteLog("%s[%d] mysql_fetch_lengths Failed\n", __FILE__, __LINE__);
		mysql_free_result(m_pQueryRes);
		return;
	}

	int		nColIndex	= 0;

	RDB_RESPOND_EXCHANGE_GIFT_RESULT	tagInfo;
	memset(&tagInfo, 0, sizeof(tagInfo));
	tagInfo.byProtocol	= rdb_respond_exchange_gift_result;
	tagInfo.uPlayerID	= pInfo->uPlayerID;
	tagInfo.byResult	= (BYTE)strtoul(m_pRow[nColIndex++], NULL, 10);

	if (m_pRow[nColIndex])
	{
		int	nDataLen	= min(sizeof(tagInfo.tagItemList), pRowLength[nColIndex]);
		memcpy(tagInfo.tagItemList, m_pRow[nColIndex], nDataLen);
		tagInfo.byItemCount	= nDataLen / sizeof(SItem);
	}

	m_pRBRoleDBRespond->SndPack(&tagInfo, sizeof(tagInfo));

	mysql_free_result(m_pQueryRes);

	while (!mysql_next_result(m_pDBHandle))
	{
		MYSQL_RES	*pResult = mysql_store_result(m_pDBHandle);
		mysql_free_result(pResult);
	}
}

void CRoleDBThread::RecvQueryDnu(const void *pPack, const unsigned int uiPackLen)
{
	ROLE_DB_QUERY_DNU	*pInfo		= (ROLE_DB_QUERY_DNU*)pPack;
	char				*pszPos		= m_strSQL;
	int					nStrLen		= 0;
	unsigned int		uLeftSize	= sizeof(m_strSQL);
	int					nLength		= 0;

	nStrLen = snprintf(pszPos, uLeftSize, "select count(*) from role where TO_DAYS(CreateTime) = TO_DAYS(from_unixtime(%d))", pInfo->nDateTime);
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}

	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	if (!Query(m_strSQL, pszPos-m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]!\n", __FILE__, __LINE__, m_strSQL);
		return;
	}

	m_pQueryRes = mysql_store_result(m_pDBHandle);
	if (NULL == m_pQueryRes)
	{
		g_pFileLog->WriteLog("%s[%d] mysql_store_result Failed\n", __FILE__, __LINE__);
		return;
	}

	if (1 != mysql_num_rows(m_pQueryRes))
	{
		g_pFileLog->WriteLog("%s[%d] DNU Data Error\n", __FILE__, __LINE__);
		return;
	}

	m_pRow = mysql_fetch_row(m_pQueryRes);
	if (NULL == m_pRow)
	{
		g_pFileLog->WriteLog("%s[%d] mysql_fetch_row Failed\n", __FILE__, __LINE__);
		mysql_free_result(m_pQueryRes);
		return;
	}

	unsigned long	*pRowLength	= mysql_fetch_lengths(m_pQueryRes);
	if (!pRowLength)
	{
		g_pFileLog->WriteLog("%s[%d] mysql_fetch_lengths Failed\n", __FILE__, __LINE__);
		return;
	}
}

void CRoleDBThread::RecvQueryDau(const void *pPack, const unsigned int uiPackLen)
{
	ROLE_DB_QUERY_DAU	*pInfo		= (ROLE_DB_QUERY_DAU*)pPack;
	char				*pszPos		= m_strSQL;
	int					nStrLen		= 0;
	unsigned int		uLeftSize	= sizeof(m_strSQL);
	int					nLength		= 0;

	nStrLen = snprintf(pszPos, uLeftSize, "select count(*) from rolelogin where TO_DAYS(LoginTime) = TO_DAYS(from_unixtime(%d)) group by Account", pInfo->nDateTime);
	if (nStrLen <= 0 || nStrLen >= uLeftSize)
	{
		g_pFileLog->WriteLog("%s[%d] snprintf SQL Error\n", __FILE__, __LINE__);
		return;
	}

	pszPos		+= nStrLen;
	uLeftSize	-= nStrLen;

	if (!Query(m_strSQL, pszPos-m_strSQL))
	{
		g_pFileLog->WriteLog("%s[%d] Query Error:\n[%s]!\n", __FILE__, __LINE__, m_strSQL);
		return;
	}

	m_pQueryRes = mysql_store_result(m_pDBHandle);
	if (NULL == m_pQueryRes)
	{
		g_pFileLog->WriteLog("%s[%d] mysql_store_result Failed\n", __FILE__, __LINE__);
		return;
	}

	if (1 != mysql_num_rows(m_pQueryRes))
	{
		g_pFileLog->WriteLog("%s[%d] DNU Data Error\n", __FILE__, __LINE__);
		return;
	}

	m_pRow = mysql_fetch_row(m_pQueryRes);
	if (NULL == m_pRow)
	{
		g_pFileLog->WriteLog("%s[%d] mysql_fetch_row Failed\n", __FILE__, __LINE__);
		mysql_free_result(m_pQueryRes);
		return;
	}

	unsigned long	*pRowLength	= mysql_fetch_lengths(m_pQueryRes);
	if (!pRowLength)
	{
		g_pFileLog->WriteLog("%s[%d] mysql_fetch_lengths Failed\n", __FILE__, __LINE__);
		return;
	}
}
