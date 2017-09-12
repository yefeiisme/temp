#ifndef __CLIENT_GS_PROTOCOL_H_
#define __CLIENT_GS_PROTOCOL_H_

#include "commondefine.h"

enum c2s_Protocol
{
	c2s_ping,
	c2s_chat_msg,
	c2s_broad_cast_msg,

	c2s_end,
};

enum s2c_Protocol
{
	s2c_chat_msg,

	s2c_end,
};

struct C2S_CHAT_MSG : public SDefinedProtocolHead
{
	char	strMsg[1024];
};

struct C2S_BROAD_CAST_MSG : public SDefinedProtocolHead
{
	char	strMsg[1024];
};


struct S2C_CHAT_MSG : public SDefinedProtocolHead
{
	char	strMsg[1024];
};

#endif
