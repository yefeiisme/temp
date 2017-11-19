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
	int		nTime;			// 时间
	float	fLongitude;		// 现场经度
	float	fLatitude;		// 现场纬度
};

struct SSensorData
{
	WORD	wLength;		// 传感器数据长度
	BYTE	byType;			// 传感器类型
	WORD	wID;			// 传感器ID
};

// 传感器变长的数据，你看一下怎么定义

// CRC校验
struct SCRCSum
{
	WORD	wCRC;
};

#pragma pack(pop)

#endif
