#ifndef __PLAYER_H_
#define __PLAYER_H_

#include "ClientGSProtocol.h"

class CServerConnection;

class CPlayer
{
public:
	CPlayer();
	~CPlayer();

	void					DoAction();
	void					ProcessNetPack(const void *pPack, const unsigned int uPackLen);
private:
	void					RecvChatMsg(const void *pPack, const unsigned int uPackLen);

private:
private:
	typedef void			(CPlayer::*pfnProtocolFunc)(const void *pPack, const unsigned int uPackLen);
	static pfnProtocolFunc	m_ProtocolFunc[s2c_end];

	CServerConnection		*m_pConnection;

	UINT					m_uUniqueID;

	time_t					m_nActionTime;
};

#endif
