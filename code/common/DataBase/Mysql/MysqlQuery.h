#ifndef __ROLE_DB_THREAD_H_
#define __ROLE_DB_THREAD_H_

#include "IRingBuffer.h"
#include "IMysqlQuery.h"
#include <string>
#include <thread>

using namespace std;

class CProcObj;
class CMysqlResult;

class CMysqlQuery : public IMysqlQuery
{
private:
	IIniFile				*m_pIniFile;

	IRingBuffer				*m_pRBRequest;
	IRingBuffer				*m_pRBRespond;

	CProcObj				*m_pProcSqlObj;
	CMysqlResult			*m_pResult;

	MYSQL					*m_pDBHandle;
	MYSQL_RES				*m_pQueryRes;
	MYSQL_ROW				m_pRow;

	char					*m_pResultBuffer;
	SMysqlRespond			*m_pRespond;
	SMysqlDataHead			*m_pDataHead;

	// Sql Result
	UINT					m_uRowCount;
	UINT					m_uColCount;

	// RingBuffer Setting
	UINT					m_uSqlBufferLen;
	UINT					m_uMaxSqlLen;
	UINT					m_uResultBufferLen;
	UINT					m_uMaxResultLen;

	UINT					m_uNextPingTime;
	UINT					m_uNextConnectTime;
	UINT					m_uSleepTime;
	UINT					m_uFrame;

	UINT					m_uPingInterval;
	UINT					m_uReconnectInterval;

	time_t					m_nTimeNow;

	unsigned short			m_usDBPort;

	string					m_strDBIP;
	string					m_strUserName;
	string					m_strPassword;
	string					m_strDBName;
	string					m_strCharacterSet;

	bool					m_bRunning;
	bool					m_bExit;
public:
	CMysqlQuery();
	~CMysqlQuery();

	inline void				Stop()
	{
		m_bRunning	= false;
	}

	inline bool				IsExit()
	{
		return m_bExit;
	}

	void					Release();
	bool					Initialize(const char *pstrSettingFile, const char *pstrSection);

	void					PrepareProc(const char *pstrProcName);
	bool					AddParam(const int nParam);
	bool					AddParam(const unsigned int uParam);
	bool					AddParam(const short sParam);
	bool					AddParam(const unsigned short usParam);
	bool					AddParam(const unsigned char byParam);
	bool					AddParam(const char *pstrParam);
	bool					AddParam(const void *pParam);
	bool					EndPrepareProc(SMysqlRequest &tagRequest);
	bool					CallProc();
	void					Query(const void *pPack, const unsigned int uPackLen);

	const void				*GetDBRespond(unsigned int &uPackLen);
	IQueryResult			*GetQueryResult();
private:
	inline void				yield()
	{
#if defined (WIN32) || defined (WIN64)
		Sleep(m_uSleepTime);
#else
		struct timeval sleeptime;
		sleeptime.tv_sec	= 0;
		sleeptime.tv_usec	= m_uSleepTime * 1000;
		select(0, 0, 0, 0, &sleeptime);
#endif
	}

	bool					LoadConfig(const char *pstrSettingFile, const char *pstrSection);
	void					DBThreadFunc();
	void					DBActive();
	void					ProcessRequest();

	void					ExecuteSQL(SMysqlRequest &pRequest, const char *pstrSQL, const unsigned int uSQLLen);
	bool					HandleResult(SMysqlRequest &pRequest);
	void					GetProcRet(MYSQL_RES *pRes);
	void					ClearResult();
	void					Disconnect();
};

#endif
