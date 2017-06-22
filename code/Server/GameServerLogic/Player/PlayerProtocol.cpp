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
	C2S_BROAD_CAST_MSG	*pRecvInfo	= (C2S_BROAD_CAST_MSG*)pPack;

	ClientGSPack::C2S_CHAT_MESSAGE	tagMsg;
	tagMsg.set_strmsg(pRecvInfo->strMsg);

	S2C_CHAT_MSG		tagSendInfo;
	memset(&tagSendInfo, 0, sizeof(tagSendInfo));
	tagSendInfo.byProtocol	= s2c_chat_msg;
	strncpy(tagSendInfo.strMsg, pRecvInfo->strMsg, sizeof(tagSendInfo.strMsg));
	tagSendInfo.strMsg[sizeof(tagSendInfo.strMsg)-1]	= '\0';

	g_pGameServerLogic.BroadCastAllPlayer(&tagSendInfo, sizeof(tagSendInfo));
}
