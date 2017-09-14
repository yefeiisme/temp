#ifndef __SENSOR_DB_OPERATION_H_
#define __SENSOR_DB_OPERATION_H_

enum RESPOND_TYPE
{
	GLOBAL_QUERY,
	APP_CLIENT,
	WEB_CLIENT,
	DATA_CLIENT,

	CLIENT_TYPE_MAX,
};

enum E_SensorDBOperation
{
	SENSOR_DB_VERIFY_ACCOUNT,
	SENSOR_DB_SLOPE_LIST,
	SENSOR_DB_SENSOR_LIST,
	SENSOR_DB_SENSOR_HISTORY,
	SENSOR_DB_LOAD_ALL_LIST,

	SENSOR_DB_OPT_MAX,
};

struct SMysqlRequest
{
	uint64	uClientID;
	UINT	uClientIndex;
	BYTE	byClientType;
	BYTE	byOpt;
};

struct SMysqlRespond
{
	uint64	uClientID;
	UINT	uClientIndex;
	UINT	uRowCount;
	UINT	uColCount;
	int		nRetCode;
	BYTE	byClientType;
	BYTE	byOpt;
};

#endif
