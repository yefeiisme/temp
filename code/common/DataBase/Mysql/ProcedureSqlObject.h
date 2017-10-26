#ifndef __PROCEDURE_SQL_OBJECT_H_
#define __PROCEDURE_SQL_OBJECT_H_

#include "IMysqlQuery.h"

class CProcObj
{
public:
	CProcObj();
	~CProcObj();

	bool					Initialize(const UINT uQueryBufferLen);

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

	void					Clear();

	inline const char		*GetRequest(UINT &uSQLLen)
	{
		uSQLLen = m_wMaxCallbackDataLen + m_uSQLLen;

		return m_pstrBuffer;
	}

	inline void				SetDBHandle(MYSQL *pDBHandle)
	{
		m_pDBHandle	= pDBHandle;
	}
private:
	MYSQL					*m_pDBHandle;

	char					*m_pstrBuffer;
	WORD					*m_pCallbackDataLen;
	char					*m_pstrCallbackData;
	char					*m_pstrSQL;

	UINT					m_uBufferLen;
	UINT					m_uMaxSQLLen;
	UINT					m_uSQLLen;

	WORD					m_wMaxCallbackDataLen;

	bool					m_bAddParam;
};

#endif
