#ifndef __MYSQL_RESULT_H_
#define __MYSQL_RESULT_H_

#include "IMysqlQuery.h"

class CMysqlResult
{
public:
	CMysqlResult();
	~CMysqlResult();

	bool					Initialize(const UINT uBufferLen);
	void					Clear();

	inline void				SetResultHead(SMysqlRequest &pRequest)
	{
		m_pResultHead->uClientID	= pRequest.uClientID;
		m_pResultHead->uClientIndex	= pRequest.uClientIndex;
		m_pResultHead->byClientType	= pRequest.byClientType;
		m_pResultHead->byOpt		= pRequest.byOpt;
	}

	inline bool				SetResultSize(const UINT uRow, const UINT uCol)
	{
		if (sizeof(SMysqlRespond)+sizeof(SMysqlDataHead)*uRow*uCol > m_uBufferLen)
			return false;

		m_uRowCount	= uRow;
		m_uColCount	= uCol;

		m_pResultHead->uRowCount	= uRow;
		m_pResultHead->uColCount	= uCol;

		return true;
	}

	inline void				SetResultCode(const int nRetCode)
	{
		m_pResultHead->nRetCode = nRetCode;
	}
	bool					AddResult(const UINT uRow, const UINT uCol, const char *pstrData, const UINT uDataLen);
private:
	char					*m_pDataBuffer;

	SMysqlRespond			*m_pResultHead;
	SMysqlDataHead			*m_pDataHead;

	UINT					m_uBufferLen;
	UINT					m_uRowCount;
	UINT					m_uColCount;

	UINT					m_uOffset;
};

#endif
