#ifndef __DATA_SERVER_PROTOCOL_H_
#define __DATA_SERVER_PROTOCOL_H_

typedef unsigned char		BYTE;
typedef unsigned short		WORD;
typedef unsigned int		UINT;

#pragma	pack(push, 1)

// 包头：表示后续整个包体的长度
struct SProtocolSize
{
	WORD	wSize;
};

// 
struct SProtocolHead
{
	WORD	wProtocolHead;	// 7E81
	BYTE	bySlopeType;	// 现场类型ID
	WORD	wSceneID;			// 现场ID
	BYTE	bySensorCount;	// 本现场传感器数量
	UINT	uTime;				// 时间
	float	fLatitude;		// 现场纬度
	float	fLongitude;		// 现场经度
};

struct SSensorHead
{
	WORD	wLength;		// 传感器数据长度
	BYTE	byType;			// 传感器类型
	BYTE	byID;			// 传感器现场ID
};

struct SSensorData20		//RDK=1----应该改为20！
{
	double	dX;			//经度
	double	dY;			//维度
	float	fZ;			//高程
	BYTE	byFix;		//FIX
	BYTE	byData[1];  //保留
};

struct SSensorData30		//485_测斜仪_s100		
{
	short	sX;			//x倾角
	short	sY;			//y倾角
	BYTE	byData[4];   //保留
};

struct SSensorData31	//485_雨量计
{
	long	lYL_Num;		//雨量lL
	BYTE	byData[4];  //保留
};

struct SSensorData32	//485_超声液位计
{
	short	sL;		//液位sL
	short	sH;		//空距sH
	BYTE	byData[4];  //保留
};

struct SSensorData35			//485_测斜仪_s3300
{
	short	sX;			//x倾角
	short	sY;			//y倾角
	BYTE	byData[4];  //保留
};

struct SSensorData4x	//振弦_土压力计
{
	short	sFi;			//频率读数
	float	fP;				//压强值P=K*(fi*fi-fo*fo),系数K 取自Sensor表中对应传感器之P1,频率fo=校准时的fi，在校准时存入Sensor之P2,计算时取出;
	BYTE	byData[2];//保留
};

/*
struct SSensorData41	//振弦_锚索计，直接显示频率，
{
	short	sFi;		//频率读数f0
	float	fP;			//载荷值P=K*(fi*fi-fo*fo),系数K 取自Sensor表中对应传感器之P1,频率fo，=校准时的fi，在校准时存入Sensor之P2,计算时取出;
	BYTE	byData[2];//保留
};

struct SSensorData42	//振弦_渗压计
{
	short	sFi;			//频率读数
	float	fW;				//水位值W=100*K*(fi*fi-fo*fo)(m),系数K 取自Sensor表中对应传感器之P1,频率fo，=校准时的fi，在校准时存入Sensor之P2,计算时取出;
	BYTE	byData[2];//保留
};
*/
// CRC校验
struct SCRCSum
{
	WORD	wCRC;
};

#pragma pack(pop)

#endif
