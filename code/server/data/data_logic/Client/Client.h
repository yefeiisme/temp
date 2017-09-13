#ifndef __LOGIC_CLIENT_H_
#define __LOGIC_CLIENT_H_

#include "IClientConnection.h"

class CClient
{
protected:
public:
	CClient();
	~CClient();

	inline void				SetIndex(const UINT uIndex)
	{
		m_uIndex	= uIndex;
	}

	inline uint64			GetUniqueID() const
	{
		return m_uUniqueID;
	}

	inline void				Login(IClientConnection *pClientConn, const uint64 uID)
	{
		m_pClientConn	= pClientConn;
		m_uUniqueID		= uID;
	}
	inline void				Logout()
	{
		m_pClientConn	= nullptr;
		m_uUniqueID		= 0;
		m_uAccountID	= 0;
	}

	virtual void			DoAction();
protected:
	IClientConnection		*m_pClientConn;
	uint64					m_uUniqueID;
	UINT					m_uIndex;
	UINT					m_uAccountID;
};

#endif
