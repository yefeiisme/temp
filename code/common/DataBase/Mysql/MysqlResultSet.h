#ifndef __MYSQL_RESULT_SET_H_
#define __MYSQL_RESULT_SET_H_

#include <vector>
#include "IMysqlQuery.h"

using namespace std;

class CMysqlResult;
class CMysqlQuery;

class CMysqlResultSet : public IMysqlResultSet
{
public:
	CMysqlResultSet(CMysqlQuery &pMysqlQuery);
	~CMysqlResultSet();
public:
	inline char				*GetCallbackData()
	{
		return m_pResultSetHead->strCallBackDta;
	}

	inline WORD				GetCallbackDataLen()
	{
		return m_pResultSetHead->wCallBackDataLen;
	}

	inline BYTE				GetResultCount()
	{
		return m_pResultSetHead->byResultCount;
	}

	inline IMysqlResult		*GetMysqlResult(const BYTE byIndex)
	{
		return (byIndex >= m_pResultSetHead->byResultCount ? nullptr : &m_pMysqlResultList[byIndex]);
	}

	bool					Initialize(const BYTE byMaxResultCount);
	void					Clear();

	inline void				AddResult(CMysqlResult *pResult)
	{
		if (nullptr == pResult)
			return;

		m_vectMysqlResult.push_back(pResult);
	}

	bool					CreateResultObj(const WORD wObjCount);
	bool					ParseResult(const void *pPack, const UINT uPackLen);
private:
	CMysqlQuery				&m_pMysqlQuery;

	CMysqlResult			*m_pMysqlResultList;

	SResultSetHead			*m_pResultSetHead;
	SResultHead				*m_pResultHead;
	char					*m_pResultData;

	BYTE					m_byMaxResultCount;

	vector<CMysqlResult*>	m_vectMysqlResult;
};

#endif
