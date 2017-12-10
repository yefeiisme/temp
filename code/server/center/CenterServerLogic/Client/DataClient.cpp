#include "stdafx.h"
#include <time.h>
#include "DataClient.h"
#include "ICenterServer.h"
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
	if (pCallbackData->byOpt >= SENSOR_DB_OPT_MAX)
	{
		g_pFileLog->WriteLog("[%s][%d] DB Respond Invalid Protocol[%hhu]\n", __FILE__, __LINE__, pCallbackData->byOpt);
		return;
	}

	//(this->*m_pfnDBRespondFunc[pCallbackData->byOpt])(pResultSet, pCallbackData);
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
	SSensorHead		*pSensorHead	= (SSensorHead*)((char*)pPack + sizeof(SProtocolHead));

	if (0x7E81 != pInfo->wProtocolHead)
	{
		g_pFileLog->WriteLog("[%s][%d] Protocol Not 7E81\n", __FILE__, __LINE__);
		return;
	}

	if (0 == pInfo->bySensorCount)
	{
		g_pFileLog->WriteLog("[%s][%d] Sensor Count Is 0\n", __FILE__, __LINE__);
		return;
	}

	time_t	nTimeNow	= pInfo->uTime;
	tm		*pTimeNow	= localtime(&nTimeNow);

	g_pFileLog->WriteLog("SlopeType=%hhu SlopeID=%hu SensorCount=%hhu Time=%d-%d-%d-%d-%d-%d DayLongitude=%f Latitude=%f\n",
		pInfo->bySlopeType, pInfo->wSlopeID, pInfo->bySensorCount, pTimeNow->tm_year+1900, pTimeNow->tm_mon+1, pTimeNow->tm_mday, pTimeNow->tm_hour, pTimeNow->tm_min, pTimeNow->tm_sec, pInfo->fLongitude, pInfo->fLatitude);

	for (auto nIndex = 0; nIndex < pInfo->bySensorCount; ++nIndex)
	{
		if (1 == pSensorHead->byType)
		{
			SSensorData1	*pSensorData = (SSensorData1*)((char*)pSensorHead + sizeof(SSensorHead));
			g_pFileLog->WriteLog("Length=%hu SensorID=%hhu SensorType=%hhu SensorData1=%hd SensorData2=%hd\n", pSensorHead->wLength, pSensorHead->byID, pSensorHead->byType, pSensorData->sData1, pSensorData->sData2);
		}
		else if (2 == pSensorHead->byType)
		{
			SSensorData2	*pSensorData = (SSensorData2*)((char*)pSensorHead + sizeof(SSensorHead));
			g_pFileLog->WriteLog("Length=%hu SensorID=%hhu SensorType=%hhu SensorData1=%hd SensorData2=%hd\n", pSensorHead->wLength, pSensorHead->byID, pSensorHead->byType,  pSensorData->sData1, pSensorData->sData2);
		}
		else if (3 == pSensorHead->byType)
		{
			SSensorData3	*pSensorData = (SSensorData3*)((char*)pSensorHead + sizeof(SSensorHead));
			g_pFileLog->WriteLog("Length=%hu SensorID=%hhu SensorType=%hhu SensorData1=%f SensorData2=%f SensorDat3=%f\n", pSensorHead->wLength, pSensorHead->byID, pSensorHead->byType,  pSensorData->dValue1, pSensorData->dValue2, pSensorData->dValue3);
		}
		else
		{
			g_pFileLog->WriteLog("Length=%hu SensorID=%hhu Invalid SensorType=%hhu\n", pSensorHead->wLength, pSensorHead->byID, pSensorHead->byType);
		}

		pSensorHead = (SSensorHead*)((char*)pSensorHead + sizeof(SSensorHead) + pSensorHead->wLength);
	}
	/*
	if (sizeof(SProtocolHead)+sizeof(SSensorData)*pInfo->bySensorCount != uPackLen)
	{
		g_pFileLog->WriteLog("[%s][%d] Pack Real Length[%u], Content Calc Length[%d]\n", __FILE__, __LINE__, uPackLen, sizeof(SProtocolHead)+sizeof(SSensorData)*pInfo->bySensorCount);
		return;
	}

	IMysqlQuery	*pMysqlQuery = g_ICenterServer.GetMysqlQuery();
	if (nullptr == pMysqlQuery)
		return;

	for (auto nIndex = 0; nIndex < pInfo->bySensorCount; ++nIndex)
	{
		SMysqlRequest	tagRequest	= {0};
		tagRequest.byOpt			= SENSOR_DB_ADD_SENSOR_DATA;
		tagRequest.uClientID		= m_uUniqueID;
		tagRequest.uClientIndex		= m_uIndex;
		tagRequest.byClientType		= DATA_CLIENT;

		pMysqlQuery->PrepareProc("AddSensorData");
		pMysqlQuery->AddParam(pInfo->nTime);
		pMysqlQuery->AddParam(pSensorData->wID);
		pMysqlQuery->AddParam(pSensorData->byType);
		pMysqlQuery->AddParam(pSensorData->dValue1);
		pMysqlQuery->AddParam(pSensorData->dValue2);
		pMysqlQuery->AddParam(pSensorData->dValue3);
		pMysqlQuery->EndPrepareProc(&tagRequest, sizeof(tagRequest));

		pMysqlQuery->CallProc();
	}
	*/

	// 正式能插入数据后，将此处删除
	// ...
	m_pClientConn->PutPack(pPack, uPackLen);
}
