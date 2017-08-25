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
	typedef void				(CSensorDBConn::*pfnProtocolFunc)(SMysqlRespond &pRespond, IQueryResult *pResult);
	pfnProtocolFunc				m_ProtocolFunc[SENSOR_DB_OPT_MAX];

	typedef void				(CSensorDBConn::*pfnTypeFunc)(SMysqlRespond &pRespond, IQueryResult *pResult);
	pfnTypeFunc					m_pfnTypeFunc[CLIENT_TYPE_MAX];

	void						GlobalQuery(SMysqlRespond &pRespond, IQueryResult *pResult);
	void						AppQuery(SMysqlRespond &pRespond, IQueryResult *pResult);
	void						WebQuery(SMysqlRespond &pRespond, IQueryResult *pResult);
	void						DataQuery(SMysqlRespond &pRespond, IQueryResult *pResult);

	void						RecvVerifyAccount(SMysqlRespond &pRespond, IQueryResult *pResult);
	void						RecvSlopeList(SMysqlRespond &pRespond, IQueryResult *pResult);
	void						RecvSensorList(SMysqlRespond &pRespond, IQueryResult *pResult);
	void						RecvSensorHistory(SMysqlRespond &pRespond, IQueryResult *pResult);
	void						RecvLoadAllList(SMysqlRespond &pRespond, IQueryResult *pResult);
};

#endif
