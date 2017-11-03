#ifndef __DATA_SERVER_PROTOCOL_H_
#define __DATA_SERVER_PROTOCOL_H_

typedef unsigned char		BYTE;		//%hhu
typedef unsigned short		WORD;		//%hu
typedef unsigned int		UINT;		//%u
#if defined(WIN32) || defined(WIN64)
typedef unsigned __int64	uint64;		//%llu
typedef __int64				int64;		//%lld
#elif defined(_linux)
typedef unsigned long long	uint64;
typedef long long			int64;
#endif

struct SDefinedProtocolHead
{
	BYTE	byProtocolHead;
};

enum d2s_Protocol
{
	d2s_ping,
	d2s_login,

	c2s_end,
};

enum s2d_Protocol
{
	d2s_login_result,

	s2c_end,
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
