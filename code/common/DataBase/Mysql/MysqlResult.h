#ifndef __MYSQL_RESULT_H_
#define __MYSQL_RESULT_H_

#include "IMysqlQuery.h"

class CMysqlResult : public IQueryResult
{
public:
	CMysqlResult();
	~CMysqlResult();
public:
	char					*GetDataString(const UINT uRow, const UINT uCol, unsigned int &uSize);
	bool					GetData(const UINT uRow, const UINT uCol, int &nData);
	bool					GetData(const UINT uRow, const UINT uCol, unsigned int &uData);
	bool					GetData(const UINT uRow, const UINT uCol, short &sData);
	bool					GetData(const UINT uRow, const UINT uCol, unsigned short &wData);
	bool					GetData(const UINT uRow, const UINT uCol, unsigned char &byData);
	bool					GetData(const UINT uRow, const UINT uCol, double &dData);
	UINT					GetData(const UINT uRow, const UINT uCol, char *pstrParam, const unsigned int uSize);
	UINT					GetData(const UINT uRow, const UINT uCol, void *pParam, const unsigned int uSize);
	inline SMysqlRespond	&GetRespond()
	{
		return *m_pResultHead;
	}

	inline SMysqlDataHead	*GetDataHead()
	{
		return m_pDataHead;
	}

	inline char				*GetDataPtr()
	{
		return m_pData;
	}
public:
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

		m_pResultHead->uRowCount	= uRow;
		m_pResultHead->uColCount	= uCol;

		m_pData	= m_pBuffer + sizeof(SMysqlRespond)+sizeof(SMysqlDataHead)*uRow*uCol;

		return true;
	}

	inline void				SetResultCode(const int nRetCode)
	{
		m_pResultHead->nRetCode = nRetCode;
	}

	bool					AddResult(const UINT uRow, const UINT uCol, const char *pstrData, const UINT uDataLen);
	bool					ParseResult(const void *pPack, const UINT uPackLen);
private:
	char					*m_pBuffer;
	char					*m_pData;

	SMysqlRespond			*m_pResultHead;
	SMysqlDataHead			*m_pDataHead;

	UINT					m_uBufferLen;
	UINT					m_uRowCount;
	UINT					m_uColCount;

	UINT					m_uOffset;
};

#endif
