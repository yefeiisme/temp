#include "stdafx.h"
#include "DataClient.h"
#include "../SensorDB/SensorDBOperation.h"

CDataClient::pfnProtocolFunc CDataClient::m_ProtocolFunc[d2s_end] =
{
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

		//if (byProtocol >= d2s_end)
		//{
		//	g_pFileLog->WriteLog("[%s][%d] Data Client[%u] Invalid Protocol[%hhu]\n", __FILE__, __LINE__, m_uUniqueID, byProtocol);
		//	return;
		//}

		RecvAddSensorData(pPack, uPackLen);
		//(this->*m_ProtocolFunc[byProtocol])(pPack, uPackLen);

		m_pClientConn->ResetTimeOut();
	};
}

void CDataClient::RecvAddSensorData(const void *pPack, const unsigned int uPackLen)
{
	SProtocolHead	*pInfo			= (SProtocolHead*)pPack;
	SSensorData		*pSensorData	= (SSensorData*)((char*)pPack + sizeof(SProtocolHead));

	g_pFileLog->WriteLog("[%s][%d] CDataClient::RecvAddSensorData\n", __FILE__, __LINE__);
}
