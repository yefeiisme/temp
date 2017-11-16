#include "stdafx.h"
#include "DataClient.h"
#include "../SensorDB/SensorDBOperation.h"

CDataClient::pfnProtocolFunc CDataClient::m_ProtocolFunc[d2s_end] =
{
	&CDataClient::RecvPing,
	&CDataClient::RecvAddSensorData,
};

CDataClient::CDataClient() : CClient()
{
}

CDataClient::~CDataClient()
{
}

void CDataClient::DoAction()
{
	ProcessNetPack();
}

void CDataClient::ProcessDBPack(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData)
{
}

void CDataClient::ProcessNetPack()
{
	const void		*pPack = nullptr;
	unsigned int	uPackLen = 0;
	BYTE			byProtocol = 0;

	while (nullptr != (pPack = m_pClientConn->GetPack(uPackLen)))
	{
		byProtocol = *((BYTE*)pPack);

		if (byProtocol >= d2s_end)
		{
			g_pFileLog->WriteLog("[%s][%d] Data Client[%u] Invalid Protocol[%hhu]\n", __FILE__, __LINE__, m_uUniqueID, byProtocol);
			return;
		}

		(this->*m_ProtocolFunc[byProtocol])(pPack, uPackLen);

		m_pClientConn->ResetTimeOut();
	};
}

void CDataClient::RecvPing(const void *pPack, const unsigned int uPackLen)
{
}

void CDataClient::RecvAddSensorData(const void *pPack, const unsigned int uPackLen)
{
	D2S_ADD_SENSOR_DATA	*pInfo			= (D2S_ADD_SENSOR_DATA*)pPack;
	SSensorData			*pSensorData	= (SSensorData*)((char*)pPack+sizeof(D2S_ADD_SENSOR_DATA));
}
