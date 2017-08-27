#ifndef __DATA_CLIENT_H_
#define __DATA_CLIENT_H_

#include "IMysqlQuery.h"
#include "Client.h"
#include "../SensorDB/SensorDBOperation.h"

class CDataClient : public CClient
{
public:
	CDataClient();
	~CDataClient();

	void					DoAction();
	void					ProcessDBPack(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData);
private:
	void					ProcessNetPack();

	void					RecvPing(const void *pPack, const unsigned int uPackLen);
private:
	typedef void			(CDataClient::*pfnProtocolFunc)(const void *pPack, const unsigned int uPackLen);
	static pfnProtocolFunc	m_ProtocolFunc[256];
private:
};

#endif
