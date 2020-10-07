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

//=====================================================
// ���ܣ���ѭ�������������ʵ��ʱ��������Ĵ˺����������ڸ����Ӷ���������紦��Ĳ���
//
void CDataClient::DoAction()
{
	ProcessNetPack();
}

//=====================================================
// ���ܣ��������ݿ��̷߳��ص����ݡ������Ա����ָ��������������صĺ������д���
// ��ע����������ֻ�����ݿ��в������ݣ�����Ҫ���صĴ���
void CDataClient::ProcessDBPack(IMysqlResultSet *pResultSet, SMysqlRequest *pCallbackData)
{
	if (pCallbackData->byOpt >= SENSOR_DB_OPT_MAX)
	{
		g_pFileLog->WriteLog("[%s][%d] DB Respond Invalid Protocol[%hhu]\n", __FILE__, __LINE__, pCallbackData->byOpt);
		return;
	}

	//(this->*m_pfnDBRespondFunc[pCallbackData->byOpt])(pResultSet, pCallbackData);
}

//=====================================================
// ���ܣ�������������õ������Ա����ָ��������������صĺ������д���
// ��ע��������ֻ��һ��Э�飬����Ҫ�õ���Ա������ָ������
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

//=====================================================
// ���ܣ����յ����������ݵĴ���
//
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
		m_pClientConn->PutPack(pPack, uPackLen);
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
		case 31:
		case 32:
		case 33:
		case 43:
			{
				SSensorData1	*pSensorData = (SSensorData1*)((char*)pSensorHead + sizeof(SSensorHead));
				dValue1	= pSensorData->dValue1;
				dValue2	= pSensorData->dValue2;
				dValue3	= pSensorData->fValue3;
				dValue4	= pSensorData->byValue4;

				// ��ʱΪ��ͳһ���ĵ���
				BYTE	byType = pSensorHead->byType + 1;

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
				pMysqlQuery->AddParam(byType);
				pMysqlQuery->AddParam(dValue1);
				pMysqlQuery->AddParam(dValue2);
				pMysqlQuery->AddParam(dValue3);
				pMysqlQuery->AddParam(dValue4);
				pMysqlQuery->EndPrepareProc(&tagRequest, sizeof(tagRequest));

				pMysqlQuery->CallProc();
				g_pFileLog->WriteLog("SlopeSceneID=%hu SlopeType=%hhu Longitude=%f Latitude=%f SensorSceneID=%hhu SensorType=%hhu Value1=%f Value2=%f Value3=%f Value4=%f\n", pInfo->wSceneID, pInfo->bySlopeType, pInfo->fLongitude, pInfo->fLatitude, pSensorHead->byID, byType, dValue1, dValue2, dValue3, dValue4);
			}
			break;
		case 2:
			{
				SSensorData2	*pSensorData = (SSensorData2*)((char*)pSensorHead + sizeof(SSensorHead));
				if (-1 != pSensorData->sData1 && -1 != pSensorData->sData2)
				{
					dValue1	= ((double)pSensorData->sData1 / 10000 - 0.95) / 0.95 * 30;
					dValue2	= ((double)pSensorData->sData2 / 10000 - 0.95) / 0.95 * 30;
					dValue3	= 0.0;
					dValue4	= 0.0;

					// ��ʱΪ��ͳһ���ĵ���
					BYTE	byType = pSensorHead->byType + 1;

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
					pMysqlQuery->AddParam(byType);
					pMysqlQuery->AddParam(dValue1);
					pMysqlQuery->AddParam(dValue2);
					pMysqlQuery->AddParam(dValue3);
					pMysqlQuery->AddParam(dValue4);
					pMysqlQuery->EndPrepareProc(&tagRequest, sizeof(tagRequest));

					pMysqlQuery->CallProc();
					g_pFileLog->WriteLog("SlopeSceneID=%hu SlopeType=%hhu Longitude=%f Latitude=%f SensorSceneID=%hhu SensorType=%hhu Value1=%f Value2=%f Value3=%f Value4=%f\n", pInfo->wSceneID, pInfo->bySlopeType, pInfo->fLongitude, pInfo->fLatitude, pSensorHead->byID, byType, dValue1, dValue2, dValue3, dValue4);
				}
			}
			break;
		case 3:
			{
				SSensorData3	*pSensorData = (SSensorData3*)((char*)pSensorHead + sizeof(SSensorHead));
				if (-1 != pSensorData->sData1 && -1 != pSensorData->sData2)
				{
					double	dFi	= (double)pSensorData->sData1 / 10;
					double	dFo	= 1289;
					double	dK	= 2.476 / 10000000;
					dValue1		= dK*(dFi*dFi - dFo*dFo);
					dValue2		= 0.0;
					dValue3		= 0.0;
					dValue4		= 0.0;

					// ��ʱΪ��ͳһ���ĵ���
					BYTE	byType = pSensorHead->byType + 1;

					SMysqlRequest	tagRequest = {0};
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
					pMysqlQuery->AddParam(byType);
					pMysqlQuery->AddParam(dValue1);
					pMysqlQuery->AddParam(dValue2);
					pMysqlQuery->AddParam(dValue3);
					pMysqlQuery->AddParam(dValue4);
					pMysqlQuery->EndPrepareProc(&tagRequest, sizeof(tagRequest));

					pMysqlQuery->CallProc();
					g_pFileLog->WriteLog("SlopeSceneID=%hu SlopeType=%hhu Longitude=%f Latitude=%f SensorSceneID=%hhu SensorType=%hhu Value1=%f Value2=%f Value3=%f Value4=%f\n", pInfo->wSceneID, pInfo->bySlopeType, pInfo->fLongitude, pInfo->fLatitude, pSensorHead->byID, byType, dValue1, dValue2, dValue3, dValue4);
				}
			}
			break;
		case 4:
			{
			}
			break;
		default:
			g_pFileLog->WriteLog("Length=%hu SensorID=%hhu Invalid SensorType=%hhu\n", pSensorHead->wLength, pSensorHead->byID, pSensorHead->byType+1);
			break;
		}

		pSensorHead = (SSensorHead*)((char*)pSensorHead + sizeof(SSensorHead) + pSensorHead->wLength);
	}

	// ��ʽ�ܲ������ݺ󣬽��˴�ɾ��
	// ...
	m_pClientConn->PutPack(pPack, uPackLen);
}
