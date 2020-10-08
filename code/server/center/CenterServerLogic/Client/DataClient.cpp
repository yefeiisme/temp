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
// 功能：主循环遍历到此类的实例时，调用类的此函数，以用于该连接对象进行网络处理的操作
//
void CDataClient::DoAction()
{
	ProcessNetPack();
}

//=====================================================
// 功能：处理数据库线程返回的数据。用类成员函数指针数组来调用相关的函数进行处理
// 备注：传感器访只往数据库中插入数据，不需要返回的处理
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
// 功能：处理网络包，用的是类成员函数指针数组来调用相关的函数进行处理
// 备注：传感器只有一个协议，不需要用到成员函数的指针数组
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
// 功能：接收到传感器数据的处理
//
void CDataClient::RecvAddSensorData(const void *pPack, const unsigned int uPackLen)//pPack=集中器上传数据包（协议包），uPackLen=pPack的包长
{
	SProtocolHead	*pInfo			= (SProtocolHead*)pPack;	//pInfo<-pPack
	SSensorHead		*pSensorHead	= (SSensorHead*)((char*)pPack + sizeof(SProtocolHead));//pSensorHead=第一个传感器包头，在协议包包头之后。

	if (0x7E81 != pInfo->wProtocolHead)//如果找不到协议包头，说明pInfo不是协议包，记录log，然后退出
	{
		g_pFileLog->WriteLog("[%s][%d] Protocol Not 7E81\n", __FILE__, __LINE__);//记录日志
		return;
	}
  //到此可知pInfo是协议包
	if (0 == pInfo->bySensorCount)	//查看协议包中传感器数量，=0则说明是心跳包
	{
		m_pClientConn->PutPack(pPack, uPackLen);//什么意思？将心跳包发回给集中器！
		g_pFileLog->WriteLog("[%s][%d] Sensor Count Is 0\n", __FILE__, __LINE__);
		return;
	}
//到此可知pInfo是协议包，且是非心跳包(即传感器数据包）
	IMysqlQuery	*pMysqlQuery = g_ICenterServer.GetMysqlQuery();//建立一个查询
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

	for (auto nIndex = 0; nIndex < pInfo->bySensorCount; ++nIndex)//此循环逐一处理各传感器数据
	{
		switch (pSensorHead->byType)
		{
		case 1:		//RTK,硬件配置错了，type本为20，结果被设置为1了，此处兼容之
		case 20:	//RTK,今后硬件配置type正确后，依然可以正确解析
			{
				SSensorData20	*pSensorData = (SSensorData20 *)((char*)pSensorHead + sizeof(SSensorHead));//得到传感器数据值

				byType = 20;//pSensorHead->byType;//改为20！
				
				dValue1	= pSensorData->dX;	//RTK经度值
				dValue2	= pSensorData->dY;	//RTK纬度值
				dValue3	= pSensorData->fZ;	//RTK高程值
				dValue4	= pSensorData->byFix;//RTK FIX指标

				// 临时为了统一做的调整
//				BYTE	byType = pSensorHead->byType + 1;//没必要！
	
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
				pMysqlQuery->AddParam(byType);		//按传感器类型20入库！
				pMysqlQuery->AddParam(dValue1);		//RTK经度值入库
				pMysqlQuery->AddParam(dValue2);		//RTK纬度值入库
				pMysqlQuery->AddParam(dValue3);		//RTK高程值入库
				pMysqlQuery->AddParam(dValue4);		//RTK FIX指标入库
				pMysqlQuery->EndPrepareProc(&tagRequest, sizeof(tagRequest));

				pMysqlQuery->CallProc();
				g_pFileLog->WriteLog("SlopeSceneID=%hu SlopeType=%hhu Longitude=%f Latitude=%f SensorSceneID=%hhu SensorType=%hhu Value1=%f Value2=%f Value3=%f Value4=%f\n", pInfo->wSceneID, pInfo->bySlopeType, pInfo->fLongitude, pInfo->fLatitude, pSensorHead->byID, byType, dValue1, dValue2, dValue3, dValue4);
*/
			}
			break;
		case 30:	//485_测斜仪_S100
			{
				double PI=3.141592654;
				SSensorData30	*pSensorData = (SSensorData30 *)((char*)pSensorHead + sizeof(SSensorHead));
				byType = pSensorHead->byType;

				short sX=pSensorData->sX;												//X角读数
				short sY=pSensorData->sY;												//Y角读数
				dValue1	= asin(((double)sX-1024)/1638)*180/PI;		//X角值(°)
				dValue2	= asin(((double)sY-1024)/1638)*180/PI;		//Y角值(°)
				dValue3	= 0.0;
				dValue4	= 0.0;


			}
			break;
		case 31:	//485_雨量计
			{
				SSensorData31	*pSensorData = (SSensorData31 *)((char*)pSensorHead + sizeof(SSensorHead));
				byType = pSensorHead->byType;

				long lYl_Num=pSensorData->lYL_Num;	//雨量计数
				dValue1		= (double)lYl_Num*0.2;		//雨量值(mm)
				dValue2		= 0.0;
				dValue3		= 0.0;
				dValue4		= 0.0;

			}
			break;
		case 32:	//485_超声液位计
			{
				SSensorData32	*pSensorData = (SSensorData32 *)((char*)pSensorHead + sizeof(SSensorHead));
				byType = pSensorHead->byType;
				
				short	dYw_Num	= pSensorData->sL;		//液位读数
				short	dKj_Num	= pSensorData->sH;		//空距读数
				dValue1		= (double)dYw_Num/1000;		//液位值(m)
				dValue2		= (double)dYw_Num/1000;		//空距值(m)
				dValue3		= 0.0;
				dValue4		= 0.0;

			}
			break;
		case 35:	//485_测斜仪_S3300
			{
				SSensorData35	*pSensorData = (SSensorData35 *)((char*)pSensorHead + sizeof(SSensorHead));
				byType = pSensorHead->byType;

				short sX=pSensorData->sX;					//X角读数
				short sY=pSensorData->sY;					//Y角读数
				dValue1	= (double)sX/16384*90;		//X角值(°)
				dValue2	= (double)sY/16384*90;		//Y角值(°)
				dValue3	= 0.0;
				dValue4	= 0.0;

			}
//		case 40:	//振弦_压力传感器
//		case 41:	//振弦_锚索计
//		case 42:	//振弦_锚索计
//			{
//				SSensorData4x	*pSensorData = (SSensorData4x *)((char*)pSensorHead + sizeof(SSensorHead));
//				byType = pSensorHead->byType;
//				BYTE 	byID = pSensorHead->byID;
////根据byType和byID找Sensor表中查出振弦参数fK和校准值sf0,//**********************这个函数怎么写，我不会！@叶飞*******************！！！
////因此所有振弦传感器都一样！！
//				float K=fK;
//				short fo=sf0;
//				short fi=pSensorData->sFi;			//频率读数
//				float fP=K*(fo*fo-fi*fi);				//算出压强fP
//	      if (byType == 42)								//如果是渗压计
//	      		fP=100*fP;									//算出水位来	
//				dValue1 = (double)fi;
//				dValue2	= (double)fP;		//频率读数;				//压强值P=K*(fi*fi-fo*fo),系数K 取自Sensor表中对应传感器之P1,频率fo=校准时的fi，在校准时存入Sensor之P2,计算时取出;
//				dValue3	= 0.0;
//				dValue4	= 0.0;
//			}
//			break;
		default:
			g_pFileLog->WriteLog("Length=%hu SensorID=%hhu Invalid SensorType=%hhu\n", pSensorHead->wLength, pSensorHead->byID, pSensorHead->byType+1);
			break;
		}
//数据入库
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
//写日志
		g_pFileLog->WriteLog("SlopeSceneID=%hu SlopeType=%hhu Longitude=%f Latitude=%f SensorSceneID=%hhu SensorType=%hhu Value1=%f Value2=%f Value3=%f Value4=%f\n", pInfo->wSceneID, pInfo->bySlopeType, pInfo->fLongitude, pInfo->fLatitude, pSensorHead->byID, byType, dValue1, dValue2, dValue3, dValue4);

		pSensorHead = (SSensorHead*)((char*)pSensorHead + sizeof(SSensorHead) + pSensorHead->wLength);//指向下一个传感器字头
	}

	// 正式能插入数据后，将此处删除
	// ...
	m_pClientConn->PutPack(pPack, uPackLen);
}
