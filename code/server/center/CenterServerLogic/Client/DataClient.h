#ifndef __DATA_CLIENT_H_
#define __DATA_CLIENT_H_

#include "IMysqlQuery.h"
#include "Client.h"
#include "DataServerProtocol.h"
#include "../SensorDB/SensorDBOperation.h"


enum d2s_Protocol
{
	d2s_add_sensor_data,

	d2s_end,
};

class CDataClient : public CClient
{
public:
	CDataClient();
	~CDataClient();

	void					DoAction();
	void					ProcessDBPack(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData);
private:
	void					ProcessNetPack();

	void					RecvAddSensorData(const void *pPack, const unsigned int uPackLen);
private:
	typedef void			(CDataClient::*pfnProtocolFunc)(const void *pPack, const unsigned int uPackLen);
	static pfnProtocolFunc	m_ProtocolFunc[d2s_end];
private:
};

#endif
