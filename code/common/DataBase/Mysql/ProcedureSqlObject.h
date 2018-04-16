#ifndef __PROCEDURE_SQL_OBJECT_H_
#define __PROCEDURE_SQL_OBJECT_H_

#include "IMysqlQuery.h"

class CProcObj
{
public:
	CProcObj();
	~CProcObj();

	bool					Initialize(const uint32_t uQueryBufferLen);

	bool					BeginBatchInsert(const char *pstrProcName, void *pCallbackData, const uint16_t wDataLen);
	bool					AddColumn(const char *pstrProcName);
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

	void					Clear();

	inline const char		*GetRequest(uint32_t &uSQLLen)
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
	uint16_t					*m_pCallbackDataLen;
	char					*m_pstrCallbackData;
	char					*m_pstrSQL;

	uint32_t				m_uBufferLen;
	uint32_t				m_uMaxSQLLen;
	uint32_t				m_uSQLLen;

	uint16_t				m_wMaxCallbackDataLen;

	bool					m_bAddColumn;
	bool					m_bAddBatchValue;
	bool					m_bAddParam;
};

#endif
