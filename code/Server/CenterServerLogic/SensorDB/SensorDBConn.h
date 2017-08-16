#ifndef __SENSOR_DB_CONN_H_
#define __SENSOR_DB_CONN_H_

#include "SensorDBOperation.h"

class CSensorDBConn
{
public:
	CSensorDBConn();
	~CSensorDBConn();

	bool						Initialize();
	void						Run();
private:
	typedef void				(CSensorDBConn::*pfnProtocolFunc)(const void *pPack, const unsigned int uPackLen);
	pfnProtocolFunc				m_ProtocolFunc[SENSOR_DB_OPT_MAX];

	void						RecvVerifyAccount(const void *pPack, const unsigned int uPackLen);
	void						RecvSlopeList(const void *pPack, const unsigned int uPackLen);
	void						RecvSensorList(const void *pPack, const unsigned int uPackLen);
	void						RecvSensorHistory(const void *pPack, const unsigned int uPackLen);
};

#endif
