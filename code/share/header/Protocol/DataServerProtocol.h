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
	WORD	wSlopeID;		// 现场ID
	BYTE	bySensorCount;	// 本现场传感器数量
	UINT	uTime;			// 时间
	float	fLatitude;		// 现场纬度
	float	fLongitude;		// 现场经度
};

struct SSensorHead
{
	WORD	wLength;		// 传感器数据长度
	BYTE	byType;			// 传感器类型
	BYTE	byID;			// 传感器ID
};

struct SSensorData1
{
	double	dValue1;
	double	dValue2;
	double	dValue3;
};

struct SSensorData2
{
	short	sData1;
	short	sData2;
	BYTE	byData[4];
};

struct SSensorData3
{
	short	sData1;
	short	sData2;
	BYTE	byData[4];
};
// 传感器变长的数据，你看一下怎么定义

// CRC校验
struct SCRCSum
{
	WORD	wCRC;
};

#pragma pack(pop)

#endif
