#ifndef __SENSOR_DB_CONN_H_
#define __SENSOR_DB_CONN_H_

#include "IMysqlQuery.h"
#include "SensorDBOperation.h"

class CSensorDBConn
{
public:
	CSensorDBConn();
	~CSensorDBConn();

	bool						Initialize();
	void						Run();
private:
	void						GlobalQuery(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData);
	void						DataQuery(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData);

	void						RecvVerifyAccount(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData);
	void						RecvSlopeList(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData);
	void						RecvSensorList(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData);
	void						RecvSensorHistory(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData);
	void						RecvLoadAllList(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData);
private:
	typedef void				(CSensorDBConn::*pfnProtocolFunc)(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData);
	pfnProtocolFunc				m_ProtocolFunc[SENSOR_DB_OPT_MAX];

	typedef void				(CSensorDBConn::*pfnTypeFunc)(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData);
	pfnTypeFunc					m_pfnTypeFunc[CLIENT_TYPE_MAX];
};

#endif
