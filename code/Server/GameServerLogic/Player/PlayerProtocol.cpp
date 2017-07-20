#include "stdafx.h"
#include "ClientGSProtocol.h"
#include "Player.h"
#include "GameServerLogic.h"
#include "ClientGSProtocol.pb.h"

CPlayer::pfnProtocolFunc CPlayer::m_ProtocolFunc[c2s_end] =
{
	&CPlayer::RecvPing,
	&CPlayer::RecvChatMsg,
	&CPlayer::RecvBroadCastMsg,
};

void CPlayer::RecvPing(const void *pPack, const unsigned int uPackLen)
{
}

void CPlayer::RecvChatMsg(const void *pPack, const unsigned int uPackLen)
{
}

void CPlayer::RecvBroadCastMsg(const void *pPack, const unsigned int uPackLen)
{
	WS_CS_NET_PACK::WS2CS_LOGIN	tagRecvMsg;
	tagRecvMsg.ParseFromArray((BYTE*)pPack+sizeof(BYTE), uPackLen-sizeof(BYTE));

	WS_CS_NET_PACK::CS2WS_LOGIN_RESULT	tagSendMsg;
	tagSendMsg.set_result(1);

	BYTE	strSendBuffer[0xffff]	= {0};
	strSendBuffer[0]	= s2c_chat_msg;

	tagSendMsg.SerializeToArray(strSendBuffer+sizeof(BYTE), tagSendMsg.ByteSize());

	SendMsg(strSendBuffer, sizeof(BYTE)+tagSendMsg.ByteSize());
}
