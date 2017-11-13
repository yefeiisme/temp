#ifndef __DATA_SERVER_PROTOCOL_H_
#define __DATA_SERVER_PROTOCOL_H_

typedef unsigned char		BYTE;
typedef unsigned short		WORD;
typedef unsigned int		UINT;

struct SDefinedProtocolHead
{
	BYTE	byProtocolHead;
};

enum d2s_Protocol
{
	d2s_ping,
	d2s_login,

	d2s_end,
};

enum s2d_Protocol
{
	d2s_login_result,

	s2d_end,
};

struct D2S_LOGIN : public SDefinedProtocolHead
{
	char	strAccount[32];
	char	strPassword[16];
};

struct D2S_LOGIN_RESULT : public SDefinedProtocolHead
{
	BYTE	byResult;
};

#endif
