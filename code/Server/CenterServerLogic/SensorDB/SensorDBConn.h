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
	typedef void				(CSensorDBConn::*pfnProtocolFunc)(SMysqlRespond &pRespond, SMysqlDataHead *pDataHead, IQueryResult *pResult);
	pfnProtocolFunc				m_ProtocolFunc[SENSOR_DB_OPT_MAX];

	typedef void				(CSensorDBConn::*pfnTypeFunc)(SMysqlRespond &pRespond, SMysqlDataHead *pDataHead, IQueryResult *pResult);
	pfnTypeFunc					m_pfnTypeFunc[CLIENT_TYPE_MAX];

	void						GlobalQuery(SMysqlRespond &pRespond, SMysqlDataHead *pDataHead, IQueryResult *pResult);
	void						AppQuery(SMysqlRespond &pRespond, SMysqlDataHead *pDataHead, IQueryResult *pResult);
	void						WebQuery(SMysqlRespond &pRespond, SMysqlDataHead *pDataHead, IQueryResult *pResult);
	void						DataQuery(SMysqlRespond &pRespond, SMysqlDataHead *pDataHead, IQueryResult *pResult);

	void						RecvVerifyAccount(SMysqlRespond &pRespond, SMysqlDataHead *pDataHead, IQueryResult *pResult);
	void						RecvSlopeList(SMysqlRespond &pRespond, SMysqlDataHead *pDataHead, IQueryResult *pResult);
	void						RecvSensorList(SMysqlRespond &pRespond, SMysqlDataHead *pDataHead, IQueryResult *pResult);
	void						RecvSensorHistory(SMysqlRespond &pRespond, SMysqlDataHead *pDataHead, IQueryResult *pResult);
	void						RecvLoadAllList(SMysqlRespond &pRespond, SMysqlDataHead *pDataHead, IQueryResult *pResult);
};

#endif
