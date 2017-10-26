#ifndef __ROLE_DB_THREAD_H_
#define __ROLE_DB_THREAD_H_

#include "IRingBuffer.h"
#include "IMysqlQuery.h"
#include <string>
#include <thread>
#include <vector>

using namespace std;

class CProcObj;
class CMysqlResult;
class CMysqlResultSet;

class CMysqlQuery : public IMysqlQuery
{
private:
	IIniFile				*m_pIniFile;

	IRingBuffer				*m_pRBRequest;
	IRingBuffer				*m_pRBRespond;

	CProcObj				*m_pProcSqlObj;
	CMysqlResultSet			*m_pResultSet;

	MYSQL					*m_pDBHandle;
	MYSQL_RES				*m_pQueryRes;
	MYSQL_ROW				m_pRow;

	char					*m_pResultBuffer;
	char					*m_pCurPos;
	SResultSetHead			*m_pResultSetHead;
	SResultHead				*m_pResultHead;
	SMysqlDataHead			*m_pDataHead;

	// RingBuffer Setting
	UINT					m_uSqlBufferLen;
	UINT					m_uMaxSqlLen;
	UINT					m_uResultBufferLen;
	UINT					m_uMaxResultLen;

	UINT					m_uLeftBufferLen;

	UINT					m_uNextPingTime;
	UINT					m_uNextConnectTime;
	UINT					m_uSleepTime;
	UINT					m_uFrame;

	UINT					m_uPingInterval;
	UINT					m_uReconnectInterval;

	time_t					m_nTimeNow;

	unsigned short			m_usDBPort;

	BYTE					m_byMaxResultCount;

	string					m_strDBIP;
	string					m_strUserName;
	string					m_strPassword;
	string					m_strDBName;
	string					m_strCharacterSet;

	vector<MYSQL_RES*>		m_vectMysqlRes;

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

	bool					PrepareProc(const char *pstrProcName);
	bool					AddParam(const int nParam);
	bool					AddParam(const unsigned int uParam);
	bool					AddParam(const short sParam);
	bool					AddParam(const unsigned short usParam);
	bool					AddParam(const unsigned char byParam);
	bool					AddParam(const float fParam);
	bool					AddParam(const double dParam);
	bool					AddParam(const char *pstrParam);
	bool					AddParam(const void *pParam);
	bool					EndPrepareProc(void *pCallbackData, const WORD wDataLen);
	bool					CallProc();

	const void				*GetDBRespond(unsigned int &uPackLen);
	IMysqlResultSet			*GetMysqlResultSet();
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

	void					ExecuteSQL(const void *pPack, const unsigned int uPackLen);
	bool					HandleResult(const void *pCallbackData, const WORD wDataLen);
	bool					AddResult();
	bool					AddResultData(const UINT uRow, const UINT uCol, const void *pData, const UINT uDataLen, UINT &uOffset);
	void					ClearResult();
	void					Disconnect();
};

#endif
