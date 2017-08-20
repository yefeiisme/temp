#ifndef __PROCEDURE_SQL_OBJECT_H_
#define __PROCEDURE_SQL_OBJECT_H_

#include "IMysqlQuery.h"

class CMysqlQuery;

class CProcObj
{
public:
	CProcObj(CMysqlQuery &pMysqlQuery);
	~CProcObj();

	bool					Initialize(const UINT uQueryBufferLen, MYSQL &pDBHandle);

	void					PrepareProc(const char *pstrProcName);
	bool					AddParam(const int nParam);
	bool					AddParam(const unsigned int uParam);
	bool					AddParam(const short sParam);
	bool					AddParam(const unsigned short usParam);
	bool					AddParam(const char cParam);
	bool					AddParam(const unsigned char byParam);
	bool					AddParam(const char *pstrParam);
	bool					AddParam(const void *pParam);
	bool					EndPrepareProc(SMysqlRequest &tagRequest);
	bool					CallProc();
	bool					Query(const void *pPack, const unsigned int uPackLen);

	void					Clear();

	inline const char		*GetRequest(UINT &uRequestLen)
	{
		uRequestLen = m_uRequestLen;

		return m_strBuffer;
	}
private:
	CMysqlQuery				&m_pQuery;
	MYSQL					*m_pDBHandle;

	char					*m_strBuffer;
	SMysqlRequest			*m_pRequest;
	char					*m_strSQL;

	UINT					m_uBufferLen;
	UINT					m_uMaxSQLLen;
	UINT					m_uRequestLen;

	bool					m_bAddParam;
};

#endif
