#ifndef __USER_H_
#define __USER_H_

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

	inline void				AttachClient(IClientConnection *pClientConn)
	{
		m_pClientConn	= pClientConn;
	}

	inline void				DetachClient()
	{
		m_pClientConn	= nullptr;
		m_uUniqueID		= 0;
	}

	inline void				SetUniqueID(const uint64 uID)
	{
		m_uUniqueID	= uID;
	}

	inline uint64			GetUniqueID() const
	{
		return m_uUniqueID;
	}

	virtual void			DoAction();
protected:
	IClientConnection		*m_pClientConn;
	uint64					m_uUniqueID;
	UINT					m_uIndex;
	UINT					m_uAccountID;
};

#endif
