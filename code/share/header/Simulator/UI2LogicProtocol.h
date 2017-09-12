#ifndef __UI_2_LOGIC_PROTOCOL_H_
#define __UI_2_LOGIC_PROTOCOL_H_

#include "commondefine.h"

enum c2s_Protocol
{
	u2l_app_login,
	u2l_web_login,
	u2l_app_slope_list,
	u2l_web_slope_list,
	u2l_app_sensor_list,

	u2l_web_sensor_list,
	u2l_app_sensor_history,
	u2l_web_sensor_history,

	u2l_end,
};

enum s2c_Protocol
{
	l2u_app_login_result,
	l2u_web_login_result,

	l2u_end,
};

struct SDefinedProtocolHead
{
	BYTE	byProtocol;
};

struct SUnDefinedProtocolHead
{
	BYTE	byProtocol;
	WORD	wSize;
};

struct U2L_APP_LOGIN : public SDefinedProtocolHead
{
	char	strAccount[64];
	char	strPassword[64];
};

struct U2L_WEB_LOGIN : public SDefinedProtocolHead
{
	char	strAccount[64];
	char	strPassword[64];
};

struct U2L_APP_SLOPE_LIST : public SDefinedProtocolHead
{
	WORD	wServerID;
};

struct U2L_WEB_SLOPE_LIST : public SDefinedProtocolHead
{
	WORD	wServerID;
};

struct U2L_APP_SENSOR_LIST : public SDefinedProtocolHead
{
	WORD	wSlopeID;
};

struct U2L_WEB_SENSOR_LIST : public SDefinedProtocolHead
{
	WORD	wSlopeID;
};

struct U2L_APP_SENSOR_HISTORY : public SDefinedProtocolHead
{
	UINT	uSensorID;
	int		nBeginTime;
	int		nEndTime;
};

struct U2L_WEB_SENSOR_HISTORY : public SDefinedProtocolHead
{
	UINT	uSensorID;
	int		nBeginTime;
	int		nEndTime;
};

#endif
