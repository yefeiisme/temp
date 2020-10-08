#include "stdafx.h"
#include <math.h>
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
void CDataClient::RecvAddSensorData(const void *pPack, const unsigned int uPackLen)//pPack=�������ϴ����ݰ���Э�������uPackLen=pPack�İ���
{
	SProtocolHead	*pInfo			= (SProtocolHead*)pPack;	//pInfo<-pPack
	SSensorHead		*pSensorHead	= (SSensorHead*)((char*)pPack + sizeof(SProtocolHead));//pSensorHead=��һ����������ͷ����Э�����ͷ֮��

	if (0x7E81 != pInfo->wProtocolHead)//����Ҳ���Э���ͷ��˵��pInfo����Э�������¼log��Ȼ���˳�
	{
		g_pFileLog->WriteLog("[%s][%d] Protocol Not 7E81\n", __FILE__, __LINE__);//��¼��־
		return;
	}
  //���˿�֪pInfo��Э���
	if (0 == pInfo->bySensorCount)	//�鿴Э����д�����������=0��˵����������
	{
		m_pClientConn->PutPack(pPack, uPackLen);//ʲô��˼�������������ظ���������
		g_pFileLog->WriteLog("[%s][%d] Sensor Count Is 0\n", __FILE__, __LINE__);
		return;
	}
//���˿�֪pInfo��Э��������Ƿ�������(�����������ݰ���
	IMysqlQuery	*pMysqlQuery = g_ICenterServer.GetMysqlQuery();//����һ����ѯ
	if (nullptr == pMysqlQuery)
		return;

	time_t	nTimeNow = pInfo->uTime;
	double	dValue1			= 0.0;
	double	dValue2			= 0.0;
	double	dValue3			= 0.0;
	double	dValue4			= 0.0;
	UINT	uSlopeIndex		= 0;
	UINT	uSensorIndex	= 0;
	BYTE	byType			= 0;

	for (auto nIndex = 0; nIndex < pInfo->bySensorCount; ++nIndex)//��ѭ����һ���������������
	{
		switch (pSensorHead->byType)
		{
		case 1:		//RTK,Ӳ�����ô��ˣ�type��Ϊ20�����������Ϊ1�ˣ��˴�����֮
		case 20:	//RTK,���Ӳ������type��ȷ����Ȼ������ȷ����
			{
				SSensorData20	*pSensorData = (SSensorData20 *)((char*)pSensorHead + sizeof(SSensorHead));//�õ�����������ֵ

				byType = 20;//pSensorHead->byType;//��Ϊ20��
				
				dValue1	= pSensorData->dX;	//RTK����ֵ
				dValue2	= pSensorData->dY;	//RTKγ��ֵ
				dValue3	= pSensorData->fZ;	//RTK�߳�ֵ
				dValue4	= pSensorData->byFix;//RTK FIXָ��

				// ��ʱΪ��ͳһ���ĵ���
//				BYTE	byType = pSensorHead->byType + 1;//û��Ҫ��
	
/*
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
				pMysqlQuery->AddParam(byType);		//������������20��⣡
				pMysqlQuery->AddParam(dValue1);		//RTK����ֵ���
				pMysqlQuery->AddParam(dValue2);		//RTKγ��ֵ���
				pMysqlQuery->AddParam(dValue3);		//RTK�߳�ֵ���
				pMysqlQuery->AddParam(dValue4);		//RTK FIXָ�����
				pMysqlQuery->EndPrepareProc(&tagRequest, sizeof(tagRequest));

				pMysqlQuery->CallProc();
				g_pFileLog->WriteLog("SlopeSceneID=%hu SlopeType=%hhu Longitude=%f Latitude=%f SensorSceneID=%hhu SensorType=%hhu Value1=%f Value2=%f Value3=%f Value4=%f\n", pInfo->wSceneID, pInfo->bySlopeType, pInfo->fLongitude, pInfo->fLatitude, pSensorHead->byID, byType, dValue1, dValue2, dValue3, dValue4);
*/
			}
			break;
		case 30:	//485_��б��_S100
			{
				double PI=3.141592654;
				SSensorData30	*pSensorData = (SSensorData30 *)((char*)pSensorHead + sizeof(SSensorHead));
				byType = pSensorHead->byType;

				short sX=pSensorData->sX;												//X�Ƕ���
				short sY=pSensorData->sY;												//Y�Ƕ���
				dValue1	= asin(((double)sX-1024)/1638)*180/PI;		//X��ֵ(��)
				dValue2	= asin(((double)sY-1024)/1638)*180/PI;		//Y��ֵ(��)
				dValue3	= 0.0;
				dValue4	= 0.0;


			}
			break;
		case 31:	//485_������
			{
				SSensorData31	*pSensorData = (SSensorData31 *)((char*)pSensorHead + sizeof(SSensorHead));
				byType = pSensorHead->byType;

				long lYl_Num=pSensorData->lYL_Num;	//��������
				dValue1		= (double)lYl_Num*0.2;		//����ֵ(mm)
				dValue2		= 0.0;
				dValue3		= 0.0;
				dValue4		= 0.0;

			}
			break;
		case 32:	//485_����Һλ��
			{
				SSensorData32	*pSensorData = (SSensorData32 *)((char*)pSensorHead + sizeof(SSensorHead));
				byType = pSensorHead->byType;
				
				short	dYw_Num	= pSensorData->sL;		//Һλ����
				short	dKj_Num	= pSensorData->sH;		//�վ����
				dValue1		= (double)dYw_Num/1000;		//Һλֵ(m)
				dValue2		= (double)dYw_Num/1000;		//�վ�ֵ(m)
				dValue3		= 0.0;
				dValue4		= 0.0;

			}
			break;
		case 35:	//485_��б��_S3300
			{
				SSensorData35	*pSensorData = (SSensorData35 *)((char*)pSensorHead + sizeof(SSensorHead));
				byType = pSensorHead->byType;

				short sX=pSensorData->sX;					//X�Ƕ���
				short sY=pSensorData->sY;					//Y�Ƕ���
				dValue1	= (double)sX/16384*90;		//X��ֵ(��)
				dValue2	= (double)sY/16384*90;		//Y��ֵ(��)
				dValue3	= 0.0;
				dValue4	= 0.0;

			}
//		case 40:	//����_ѹ��������
//		case 41:	//����_ê����
//		case 42:	//����_ê����
//			{
//				SSensorData4x	*pSensorData = (SSensorData4x *)((char*)pSensorHead + sizeof(SSensorHead));
//				byType = pSensorHead->byType;
//				BYTE 	byID = pSensorHead->byID;
////����byType��byID��Sensor���в�����Ҳ���fK��У׼ֵsf0,//**********************���������ôд���Ҳ��ᣡ@Ҷ��*******************������
////����������Ҵ�������һ������
//				float K=fK;
//				short fo=sf0;
//				short fi=pSensorData->sFi;			//Ƶ�ʶ���
//				float fP=K*(fo*fo-fi*fi);				//���ѹǿfP
//	      if (byType == 42)								//�������ѹ��
//	      		fP=100*fP;									//���ˮλ��	
//				dValue1 = (double)fi;
//				dValue2	= (double)fP;		//Ƶ�ʶ���;				//ѹǿֵP=K*(fi*fi-fo*fo),ϵ��K ȡ��Sensor���ж�Ӧ������֮P1,Ƶ��fo=У׼ʱ��fi����У׼ʱ����Sensor֮P2,����ʱȡ��;
//				dValue3	= 0.0;
//				dValue4	= 0.0;
//			}
//			break;
		default:
			g_pFileLog->WriteLog("Length=%hu SensorID=%hhu Invalid SensorType=%hhu\n", pSensorHead->wLength, pSensorHead->byID, pSensorHead->byType+1);
			break;
		}
//�������
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
//д��־
		g_pFileLog->WriteLog("SlopeSceneID=%hu SlopeType=%hhu Longitude=%f Latitude=%f SensorSceneID=%hhu SensorType=%hhu Value1=%f Value2=%f Value3=%f Value4=%f\n", pInfo->wSceneID, pInfo->bySlopeType, pInfo->fLongitude, pInfo->fLatitude, pSensorHead->byID, byType, dValue1, dValue2, dValue3, dValue4);

		pSensorHead = (SSensorHead*)((char*)pSensorHead + sizeof(SSensorHead) + pSensorHead->wLength);//ָ����һ����������ͷ
	}

	// ��ʽ�ܲ������ݺ󣬽��˴�ɾ��
	// ...
	m_pClientConn->PutPack(pPack, uPackLen);
}
