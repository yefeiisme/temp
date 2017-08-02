#ifndef __ROLE_DB_THREAD_H_
#define __ROLE_DB_THREAD_H_

#include "IRingBuffer.h"
#include "ITabFile.h"
#include "IMysqlQuery.h"
#include "my_global.h"
#include "mysql.h"
#include <string>
#include <thread>

using namespace std;

#define MAX_SQL_LEN						1024*1024
#define	ROLE_DB_RB_REQUEST_LEN			16*1024*1024
#define ROLE_DB_RB_REQUEST_PACK_LEN		1024*1024
#define ROLE_DB_RB_RESPOND_LEN			16*1024*1024
#define ROLE_DB_RB_RESPOND_PACK_LEN		1024*1024

class CMysqlQuery : public IMysqlQuery
{
private:
	ITabFile				*m_pFile;
	IRingBuffer				*m_pRBRequest;
	IRingBuffer				*m_pRBRespond;

	MYSQL					*m_pDBHandle;
	MYSQL_RES				*m_pQueryRes;
	MYSQL_ROW				m_pRow;

	char					m_strSQL[MAX_SQL_LEN];
	unsigned int			m_uLastError;

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

	bool					Initialize(char *pstrDBIP, char *pstrAccount, char *pstrPassword, char *pstrDBName, unsigned short usDBPort, char *pstrCharset, unsigned int uPingTime);
	bool					SendDBRequest(const void *pPack, const unsigned int uPackLen);
	const void				*GetDBRespond(unsigned int &uPackLen);

	inline void				Stop()
	{
		m_bRunning	= false;
	}

	inline bool				IsExit()
	{
		return m_bExit;
	}

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

	void					Release();
private:
	void					DBThreadFunc();
	void					DBActive();
	void					ProcessRequest();

	bool					Query(const char *pstrSQL, const unsigned int uSQLLen);
	bool					ExecuteSQL(const char *pstrSQL);
	void					Disconnect();
};

#endif
