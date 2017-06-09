#ifndef __PLAYER_H_
#define __PLAYER_H_

#include "ClientGSProtocol.h"
#include "IClientConnection.h"

class CPlayer
{
public:
	CPlayer();
	~CPlayer();

	void					DoAction();
private:
	void					ProcessNetPack();

	void					RecvPing(const void *pPack, const unsigned int uPackLen);
	void					RecvChatMsg(const void *pPack, const unsigned int uPackLen);
	void					RecvBroadCastMsg(const void *pPack, const unsigned int uPackLen);
public:
	inline void				AttachClient(IClientConnection *pClientConnection)
	{
		m_pClientConn	= pClientConnection;
	}

	inline void				DetachClient()
	{
		m_pClientConn	= nullptr;
	}

	void					SendMsg(const void *pPack, const unsigned int uPackLen);
private:
	typedef void			(CPlayer::*pfnProtocolFunc)(const void *pPack, const unsigned int uPackLen);
	static pfnProtocolFunc	m_ProtocolFunc[c2s_end];

	IClientConnection		*m_pClientConn;

	UINT					m_uUniqueID;
};

#endif
