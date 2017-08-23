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
	typedef void				(CSensorDBConn::*pfnProtocolFunc)(SMysqlRespond &pRespond, SMysqlDataHead &pDataHead);
	pfnProtocolFunc				m_ProtocolFunc[SENSOR_DB_OPT_MAX];

	typedef void				(CSensorDBConn::*pfnTypeFunc)(SMysqlRespond &pRespond, SMysqlDataHead &pDataHead);
	pfnTypeFunc					m_pfnTypeFunc[CLIENT_TYPE_MAX];

	void						GlobalQuery(SMysqlRespond &pRespond, SMysqlDataHead &pDataHead);
	void						AppQuery(SMysqlRespond &pRespond, SMysqlDataHead &pDataHead);
	void						WebQuery(SMysqlRespond &pRespond, SMysqlDataHead &pDataHead);
	void						DataQuery(SMysqlRespond &pRespond, SMysqlDataHead &pDataHead);

	void						RecvVerifyAccount(SMysqlRespond &pRespond, SMysqlDataHead &pDataHead);
	void						RecvSlopeList(SMysqlRespond &pRespond, SMysqlDataHead &pDataHead);
	void						RecvSensorList(SMysqlRespond &pRespond, SMysqlDataHead &pDataHead);
	void						RecvSensorHistory(SMysqlRespond &pRespond, SMysqlDataHead &pDataHead);
};

#endif
