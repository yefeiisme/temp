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

	IMysqlQuery	*pMysqlQuery = g_ICenterServer.GetMysqlQuery();
	if (nullptr == pMysqlQuery)
		return;

	time_t	nTimeNow = pInfo->uTime;
	double	dValue1			= 0.0;
	double	dValue2			= 0.0;
	double	dValue3			= 0.0;
	double	dValue4			= 0.0;
	UINT	uSlopeIndex		= 0;
	UINT	uSensorIndex	= 0;

	for (auto nIndex = 0; nIndex < pInfo->bySensorCount; ++nIndex)
	{
		switch (pSensorHead->byType)
		{
		case 1:
			{
				SSensorData1	*pSensorData = (SSensorData1*)((char*)pSensorHead + sizeof(SSensorHead));
				dValue1	= pSensorData->dValue1;
				dValue2	= pSensorData->dValue2;
				dValue3	= pSensorData->fValue3;
				dValue4	= pSensorData->byValue4;

				// 临时为了统一做的调整
				++pSensorHead->byType;

				SMysqlRequest	tagRequest = {0};
				tagRequest.byOpt		= SENSOR_DB_ADD_SENSOR_DATA;
				tagRequest.uClientID	= m_uUniqueID;
				tagRequest.uClientIndex	= m_uIndex;
				tagRequest.byClientType	= DATA_CLIENT;

				pMysqlQuery->PrepareProc("AddSensorData");
				pMysqlQuery->AddParam(pInfo->wSceneID);
				pMysqlQuery->AddParam(pInfo->bySlopeType);
				pMysqlQuery->AddParam(pInfo->uTime);
				pMysqlQuery->AddParam((double)pInfo->fLongitude);
				pMysqlQuery->AddParam((double)pInfo->fLatitude);
				pMysqlQuery->AddParam(pSensorHead->byID);
				pMysqlQuery->AddParam(pSensorHead->byType);
				pMysqlQuery->AddParam(dValue1);
				pMysqlQuery->AddParam(dValue2);
				pMysqlQuery->AddParam(dValue3);
				pMysqlQuery->AddParam(dValue4);
				pMysqlQuery->EndPrepareProc(&tagRequest, sizeof(tagRequest));

				pMysqlQuery->CallProc();
				g_pFileLog->WriteLog("SlopeSceneID=%hu SlopeType=%hhu Longitude=%f Latitude=%f SensorSceneID=%hhu SensorType=%hhu Value1=%f Value2=%f Value3=%f Value4=%f\n", pInfo->wSceneID, pInfo->bySlopeType, pInfo->fLongitude, pInfo->fLatitude, pSensorHead->byID, pSensorHead->byType, dValue1, dValue2, dValue3, dValue4);
			}
			break;
		case 2:
			{
				SSensorData2	*pSensorData = (SSensorData2*)((char*)pSensorHead + sizeof(SSensorHead));
				dValue1	= ((double)pSensorData->sData1/10000-0.95)/0.95*30;
				dValue2	= ((double)pSensorData->sData2/10000-0.95)/0.95*30;
				dValue3	= 0.0;
				dValue4	= 0.0;

				if (-1 == pSensorData->sData1 || -1 == pSensorData->sData2)
					break;

				// 临时为了统一做的调整
				++pSensorHead->byType;

				SMysqlRequest	tagRequest = { 0 };
				tagRequest.byOpt		= SENSOR_DB_ADD_SENSOR_DATA;
				tagRequest.uClientID	= m_uUniqueID;
				tagRequest.uClientIndex	= m_uIndex;
				tagRequest.byClientType	= DATA_CLIENT;

				pMysqlQuery->PrepareProc("AddSensorData");
				pMysqlQuery->AddParam(pInfo->wSceneID);
				pMysqlQuery->AddParam(pInfo->bySlopeType);
				pMysqlQuery->AddParam(pInfo->uTime);
				pMysqlQuery->AddParam((double)pInfo->fLongitude);
				pMysqlQuery->AddParam((double)pInfo->fLatitude);
				pMysqlQuery->AddParam(pSensorHead->byID);
				pMysqlQuery->AddParam(pSensorHead->byType);
				pMysqlQuery->AddParam(dValue1);
				pMysqlQuery->AddParam(dValue2);
				pMysqlQuery->AddParam(dValue3);
				pMysqlQuery->AddParam(dValue4);
				pMysqlQuery->EndPrepareProc(&tagRequest, sizeof(tagRequest));

				pMysqlQuery->CallProc();
				g_pFileLog->WriteLog("SlopeSceneID=%hu SlopeType=%hhu Longitude=%f Latitude=%f SensorSceneID=%hhu SensorType=%hhu Value1=%f Value2=%f Value3=%f Value4=%f\n", pInfo->wSceneID, pInfo->bySlopeType, pInfo->fLongitude, pInfo->fLatitude, pSensorHead->byID, pSensorHead->byType, dValue1, dValue2, dValue3, dValue4);
			}
			break;
		case 3:
			{
				SSensorData3	*pSensorData = (SSensorData3*)((char*)pSensorHead + sizeof(SSensorHead));
				double	dFi	= (double)pSensorData->sData1 / 10;
				double	dFo	= 1289;
				double	dK	= 2.476 / 10000000;
				dValue1		= dK*(dFi*dFi - dFo*dFo);
				dValue2		= 0.0;
				dValue3		= 0.0;
				dValue4		= 0.0;

				if (-1 == pSensorData->sData1 || -1 == pSensorData->sData2)
					break;

				// 临时为了统一做的调整
				++pSensorHead->byType;

				SMysqlRequest	tagRequest = { 0 };
				tagRequest.byOpt = SENSOR_DB_ADD_SENSOR_DATA;
				tagRequest.uClientID = m_uUniqueID;
				tagRequest.uClientIndex = m_uIndex;
				tagRequest.byClientType = DATA_CLIENT;

				pMysqlQuery->PrepareProc("AddSensorData");
				pMysqlQuery->AddParam(pInfo->wSceneID);
				pMysqlQuery->AddParam(pInfo->bySlopeType);
				pMysqlQuery->AddParam(pInfo->uTime);
				pMysqlQuery->AddParam((double)pInfo->fLongitude);
				pMysqlQuery->AddParam((double)pInfo->fLatitude);
				pMysqlQuery->AddParam(pSensorHead->byID);
				pMysqlQuery->AddParam(pSensorHead->byType);
				pMysqlQuery->AddParam(dValue1);
				pMysqlQuery->AddParam(dValue2);
				pMysqlQuery->AddParam(dValue3);
				pMysqlQuery->AddParam(dValue4);
				pMysqlQuery->EndPrepareProc(&tagRequest, sizeof(tagRequest));

				pMysqlQuery->CallProc();
				g_pFileLog->WriteLog("SlopeSceneID=%hu SlopeType=%hhu Longitude=%f Latitude=%f SensorSceneID=%hhu SensorType=%hhu Value1=%f Value2=%f Value3=%f Value4=%f\n", pInfo->wSceneID, pInfo->bySlopeType, pInfo->fLongitude, pInfo->fLatitude, pSensorHead->byID, pSensorHead->byType, dValue1, dValue2, dValue3, dValue4);
			}
			break;
		case 4:
			{
			}
			break;
		default:
			g_pFileLog->WriteLog("Length=%hu SensorID=%hhu Invalid SensorType=%hhu\n", pSensorHead->wLength, pSensorHead->byID, pSensorHead->byType);
			break;
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
