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
	ClientGSPack::C2S_CHAT_MESSAGE	tagRecvMsg;
	tagRecvMsg.ParseFromArray((BYTE*)pPack+sizeof(BYTE), uPackLen-sizeof(BYTE));

	ClientGSPack::S2C_CHAT_MESSAGE	tagSendMsg;
	tagSendMsg.set_strmsg(tagRecvMsg.strmsg());

	BYTE	strSendBuffer[0xffff]	= {0};
	strSendBuffer[0]	= s2c_chat_msg;

	tagSendMsg.SerializeToArray(strSendBuffer+sizeof(BYTE), tagSendMsg.ByteSize());

	g_pGameServerLogic.BroadCastAllPlayer(strSendBuffer, sizeof(BYTE)+tagSendMsg.ByteSize());
}
