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
	uint32_t				m_uSqlBufferLen;
	uint32_t				m_uMaxSqlLen;
	uint32_t				m_uResultBufferLen;
	uint32_t				m_uMaxResultLen;

	uint32_t				m_uLeftBufferLen;

	uint32_t				m_uNextPingTime;
	uint32_t				m_uNextConnectTime;
	uint32_t				m_uSleepTime;
	uint32_t				m_uFrame;

	uint32_t				m_uPingInterval;
	uint32_t				m_uReconnectInterval;

	time_t					m_nTimeNow;

	unsigned short			m_usDBPort;

	uint8_t					m_byMaxResultCount;

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

	bool					BeginBatchInsert(const char *pstrTableName, void *pCallbackData, const uint16_t wDataLen);
	bool					AddColumn(const char *pstrColName);
	bool					EndColumn();
	bool					BeginAddParam();
	bool					EndAddParam();
	bool					BatchInsert();

	bool					PrepareProc(const char *pstrProcName);
	bool					AddParam(const int64_t nParam);
	bool					AddParam(const uint64_t nParam);
	bool					AddParam(const int nParam);
	bool					AddParam(const uint32_t uParam);
	bool					AddParam(const short sParam);
	bool					AddParam(const uint16_t usParam);
	bool					AddParam(const uint8_t byParam);
	bool					AddParam(const float fParam);
	bool					AddParam(const double dParam);
	bool					AddParam(const char *pstrParam);
	bool					AddParam(const void *pParam, const unsigned int uParamLen);
	bool					EndPrepareProc(void *pCallbackData, const uint16_t wDataLen);
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
	bool					HandleResult(const void *pCallbackData, const uint16_t wDataLen);
	bool					AddResult();
	bool					AddResultData(const uint32_t uRow, const uint32_t uCol, const void *pData, const uint32_t uDataLen, uint32_t &uOffset);
	void					ClearResult();
	void					Disconnect();
};

#endif
