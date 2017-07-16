#ifndef __ROLE_DB_THREAD_H_
#define __ROLE_DB_THREAD_H_

#include "RoleDBProtocol.h"
#include "IRingBuffer.h"
#include "Thread.h"
#include "IRoleDB.h"
#include "ITabFile.h"

#define MAX_SQL_LEN			1024*1024
#define	ROLE_DB_RB_REQUEST_LEN			16*1024*1024
#define ROLE_DB_RB_REQUEST_PACK_LEN		1024*1024
#define ROLE_DB_RB_RESPOND_LEN			16*1024*1024
#define ROLE_DB_RB_RESPOND_PACK_LEN		1024*1024

typedef struct __SCreateRobotInfo
{
	char				strAccount[MAX_ACCOUNT_LEN];
	char				strRoleName[MAX_PLAYER_NAME_LEN];
	int					nPower;
	WORD				wRanking;
	BYTE				byPlayerLevel;
	BYTE				byPlayerStar;
	BYTE				byHeroCount;					// 拥有的英雄数量
	BYTE				byDefendHeroCount;
	BYTE				byImagerID;
	BYTE				byOwnMagicNum;					// 拥有的法宝数量
	SDBMagicWeaponData	tagMagicData[MAX_MAGIC_NUM];	// 拥有的法宝数据
	SDBHeroData			tagHeroData[MAX_HERO_COUNT];	// 拥有的英雄数据
	SHeroRankingData	tagRankingHeroData[MAX_EM_BATTLE_HERO];
	SDBFormationAddInfo	tagDefendAddInfo;
	SDBCave				tagCave;
	SDBMagicMatrix		tagMagicMatrix;					// 法阵数据
}SCreateRobotInfo;

class CRoleDBThread : public IRoleDB
{
private:
	ITabFile					*m_pFile;
	IRingBuffer					*m_pRBRoleDBRequest;
	IRingBuffer					*m_pRBRoleDBRespond;

	MYSQL						*m_pDBHandle;
	MYSQL_RES					*m_pQueryRes;
	MYSQL_ROW					m_pRow;

	char						m_strDBIP[MAX_IP_LEN];
	unsigned short				m_usDBPort;
	char						m_strUserName[MAX_ACCOUNT_LEN];
	char						m_strPassword[MAX_PASSWORD_LEN];
	char						m_strDBName[MAX_DB_NAME_LEN];

	char						m_strSQL[MAX_SQL_LEN];
	unsigned int				m_uLastError;

	int							m_nRobotDataCount;
	SCreateRobotInfo			*m_pCreatRobotData;

	UINT						m_uNextPingTime;
	UINT						m_uNextConnectTime;

	typedef void				(CRoleDBThread::*pfnProtocolFunc)(const void *pPack, const unsigned int uiPackLen);
	static pfnProtocolFunc		m_ProtocolFunc[role_db_end];

	CThreadObj<CRoleDBThread>	m_RoleDBThread;
	bool						m_bRunning;
	bool						m_bExit;
public:
	CRoleDBThread();
	~CRoleDBThread();

	bool					Initialize(char *pstrDBIP, char *pstrAccount, char *pstrPassword, char *pstrDBName, unsigned short usDBPort);
	bool					SendRoleDBRequest(const void *pPack, const unsigned int uiPackLen);
	const void				*GetRoleDBRespond(unsigned int &uiPackLen);
	inline void				Stop()
	{
		m_bRunning	= false;
	}
	inline bool				IsExit()
	{
		return m_bExit;
	}
	void					SaveAllData();
	void					Release();
protected:
	bool					LoadRobotData();
	void					LoadNewMail(GS2CS_SEND_MAIL *pLoadInfo, const int nPostTime);
	void					LoadNewGlobalMail(const int nPostTime);
	void					DeletePlayerExpireMail();
	void					DeleteGlobalExpireMail();
private:
	void					RoleDBThreadFunc();
	void					DBActive();
	void					ProcessRequest();

	bool					IsExistDB(const char *pstrDBName);
	bool					Query(const char *pstrSQL, const unsigned int uiSQLLen);
	void					Disconnect();

	bool					CreateDataBase();
	bool					CreateGSRobotAccount(const WORD wGameServerID);
	bool					CreateGSFreeRoleName(const WORD wGameServerID, const UINT uFreeRoleNameCount);
	bool					CreateTables();
	bool					CreateProcedure();
	bool					CreateGameServerList();
	bool					CreateActivityServerList();
	bool					CreateGameMasterList();
	bool					CreateGameServer(const WORD wGSID, char (&strGSName)[MAX_SERVER_NAME_LEN], char (&strGSIP)[MAX_IP_LEN], const WORD wPlayerCount, const UINT uFreeRoleNameCount);
	bool					CreateActivityServer(const WORD wGSID, char (&strGSName)[MAX_SERVER_NAME_LEN], char (&strGSIP)[MAX_IP_LEN], const bool bLoadAS);
	bool					CreateGameMaster(char (&strAccount)[MAX_ACCOUNT_LEN], char (&strPassword)[MAX_PASSWORD_LEN], const bool bLoadGM);
	bool					RegistGameServer(const WORD wGSID, char (&strGSName)[MAX_SERVER_NAME_LEN], char (&strGSIP)[MAX_IP_LEN], const WORD wPlayerCount);
	bool					RegistActivityServer(const WORD wGSID, char (&strGSName)[MAX_SERVER_NAME_LEN], char (&strGSIP)[MAX_IP_LEN]);
	bool					RegistGameMaster(char (&strAccount)[MAX_ACCOUNT_LEN], char (&strPassword)[MAX_PASSWORD_LEN]);

	bool					ExecuteSQL(const char *pstrSQL);
	bool					LoadAllRoleData();
	bool					LoadAllGlobalMail();
	bool					LoadAllGSData();
	bool					LoadGSData(const WORD wGameServerID);
	bool					LoadAllActivityData();
	bool					LoadActivityData(const WORD wActivityServerID);
	bool					LoadAllGameMasterData();
	bool					LoadGameMasterData(char (&strAccount)[MAX_ACCOUNT_LEN]);

	bool					CreateRobotRole(const WORD wGameServerID, SCreateRobotInfo &tagCreateRobotInfo);

	bool					AddGiftCode(SGiftCode *pGiftCode);

	void					RequestCreateRole(const void *pPack, const unsigned int uiPackLen);
	void					RequestSaveRole(const void *pPack, const unsigned int uiPackLen);
	void					RequestRename(const void *pPack, const unsigned int uiPackLen);
	void					RequestCreateMail(const void *pPack, const unsigned int uiPackLen);
	void					RequestLoadMails(const void *pPack, const unsigned int uiPackLen);

	void					RequestUpdateMail(const void *pPack, const unsigned int uiPackLen);
	void					RequestDeleteMail(const void *pPack, const unsigned int uiPackLen);
	void					RequestCreateGlobalMail(const void *pPack, const unsigned int uiPackLen);
	void					RequestDeleteExpireMail(const void *pPack, const unsigned int uiPackLen);
	void					RequestCreateMailOnly(const void *pPack, const unsigned int uiPackLen);

	void					RequestUpdateMailExpire(const void *pPack, const unsigned int uiPackLen);
	void					RequestUpdateLastResetTime(const void *pPack, const unsigned int uiPackLen);
	void					RequestUpdateLastPVPAwardTime(const void *pPack, const unsigned int uiPackLen);
	void					RequestUpdateRoleTDRanking(const void *pPack, const unsigned int uiPackLen);
	void					RequestUpdateOnlinePVPPoint(const void *pPack, const unsigned int uiPackLen);

	void					RequestLastOnlinePVPAwardTime(const void *pPack, const unsigned int uiPackLen);
	void					RequestUpdateOnlinePVPTitle(const void *pPack, const unsigned int uiPackLen);
	void					RequestAddRoleLoginRecord(const void *pPack, const unsigned int uiPackLen);
	void					RecvGMLog(const void *pPack, const unsigned int uiPackLen);
	void					RecvQueryRemain(const void *pPack, const unsigned int uiPackLen);

	void					RecvAddGiftCode(const void *pPack, const unsigned int uiPackLen);
	void					RecvDelGiftCode(const void *pPack, const unsigned int uiPackLen);
	void					RecvUpdateGiftCode(const void *pPack, const unsigned int uiPackLen);
	void					RecvExchangeGiftCode(const void *pPack, const unsigned int uiPackLen);
	void					RecvQueryDnu(const void *pPack, const unsigned int uiPackLen);

	void					RecvQueryDau(const void *pPack, const unsigned int uiPackLen);
	void					RecvLastPerDayResetTime(const void *pPack, const unsigned int uiPackLen);
	void					RecvQueryCommonRemain(const void *pPack, const unsigned int uiPackLen);
	void					RecvCreateGuild(const void *pPack, const unsigned int uiPackLen);
	void					RecvApplyJoinGuild(const void *pPack, const unsigned int uiPackLen);

	void					RecvRefuseJoinGuild(const void *pPack, const unsigned int uiPackLen);
	void					RecvApproveJoinGuild(const void *pPack, const unsigned int uiPackLen);
	void					RecvPlayerJoinGuild(const void *pPack, const unsigned int uiPackLen);
	void					RecvUpdateCTRanking(const void *pPack, const unsigned int uiPackLen);
	void					RecvLoadFriendLog(const void *pPack, const unsigned int uiPackLen);

	void					RecvUpdateFriend(const void *pPack, const unsigned int uiPackLen);
	void					RecvAddFriendLog(const void *pPack, const unsigned int uiPackLen);
	void					RecvDelFriendLog(const void *pPack, const unsigned int uiPackLen);
	void					RecvCreateRedEnvelope(const void *pPack, const unsigned int uiPackLen);
	void					RecvUpdateRedEnvelope(const void *pPack, const unsigned int uiPackLen);

	void					RecvDeleteRedEnvelope(const void *pPack, const unsigned int uiPackLen);
	void					RecvAddFriendWhisper(const void *pPack, const unsigned int uiPackLen);
	void					RecvRequestFriendWhisper(const void *pPack, const unsigned int uiPackLen);
};

#endif
