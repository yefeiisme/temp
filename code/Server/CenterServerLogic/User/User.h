#ifndef __USER_H_
#define __USER_H_

#include "IClientConnection.h"

class CUser
{
public:
	CUser();
	~CUser();

	inline void				AttachClient(IClientConnection *pClientConn)
	{
		m_pClientConn	= pClientConn;
	}

	inline void				DetachClient()
	{
		m_pClientConn	= nullptr;
	}

	virtual void			DoAction();
protected:
	IClientConnection		*m_pClientConn;
	UINT					m_uUniqueID;
};

#endif
