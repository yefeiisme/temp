#include "stdafx.h"
#include "Player.h"

CPlayer::pfnProtocolFunc CPlayer::m_ProtocolFunc[s2c_end] =
{
	&CPlayer::RecvChatMsg
};

void CPlayer::RecvChatMsg(const void *pPack, const unsigned int uPackLen)
{
}
