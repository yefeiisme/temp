#include "stdafx.h"
#include "Player.h"
#include "ClientGSProtocol.pb.h"

CPlayer::pfnProtocolFunc CPlayer::m_ProtocolFunc[s2c_end] =
{
	&CPlayer::RecvChatMsg
};

void CPlayer::RecvChatMsg(const void *pPack, const unsigned int uPackLen)
{
	WS_CS_NET_PACK::CS2WS_LOGIN_RESULT	tagResult;
	tagResult.ParseFromArray((BYTE*)pPack + sizeof(BYTE), uPackLen - sizeof(BYTE));
}
