#ifndef __PROCEDURE_SQL_OBJECT_H_
#define __PROCEDURE_SQL_OBJECT_H_

#include "IMysqlQuery.h"

class CMysqlQuery;

class CProcObj
{
public:
	CProcObj(CMysqlQuery &pMysqlQuery);
	~CProcObj();

	bool			Initialize(const UINT uQueryBufferLen, MYSQL &pDBHandle);

	bool			AddParam(const int nParam);
	bool			AddParam(const unsigned int uParam);
	bool			AddParam(const short sParam);
	bool			AddParam(const unsigned short usParam);
	bool			AddParam(const char cParam);
	bool			AddParam(const unsigned char byParam);
	bool			AddParam(const char *pstrParam);
	bool			AddParam(const void *pParam);

	void			Clear();
private:
	CMysqlQuery		&m_pQuery;
	MYSQL			*m_pDBHandle;

	char			*m_strBuffer;
	SMysqlRequest	*m_pRequest;
	char			*m_strSQL;

	UINT			m_uBufferLen;
	UINT			m_uMaxSQLLen;

	bool			m_bAddParam;
};

#endif
