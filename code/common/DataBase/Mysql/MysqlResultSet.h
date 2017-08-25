#ifndef __MYSQL_RESULT_SET_H_
#define __MYSQL_RESULT_SET_H_

#include "IMysqlQuery.h"

class CMysqlResultSet : public IMysqlResultSet
{
public:
	CMysqlResultSet();
	~CMysqlResultSet();
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

	inline void				SetCallbackData(const void *pPack, const WORD wDataLen)
	{
		memcpy(m_pCallbackData, pPack, wDataLen);
		m_wCallbackDataLen	= wDataLen;
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
	void					*m_pCallbackData;

	// 以下变量，直接用指针，指向取的缓冲区指针
	// ...
	char					*m_pBuffer;
	char					*m_pData;

	SMysqlRespond			*m_pResultHead;
	SMysqlDataHead			*m_pDataHead;

	UINT					m_uBufferLen;
	UINT					m_uRowCount;
	UINT					m_uColCount;

	UINT					m_uOffset;

	int						m_nReturnCode;

	WORD					m_wResultCount;
	WORD					m_wCallbackDataLen;
};

#endif
