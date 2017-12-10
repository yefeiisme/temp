#ifndef __DATA_SERVER_PROTOCOL_H_
#define __DATA_SERVER_PROTOCOL_H_

typedef unsigned char		BYTE;
typedef unsigned short		WORD;
typedef unsigned int		UINT;

#pragma	pack(push, 1)

// ��ͷ����ʾ������������ĳ���
struct SProtocolSize
{
	WORD	wSize;
};

// 
struct SProtocolHead
{
	WORD	wProtocolHead;	// 7E81
	BYTE	bySlopeType;	// �ֳ�����ID
	WORD	wSlopeID;		// �ֳ�ID
	BYTE	bySensorCount;	// ���ֳ�����������
	UINT	uTime;			// ʱ��
	float	fLatitude;		// �ֳ�γ��
	float	fLongitude;		// �ֳ�����
};

struct SSensorHead
{
	WORD	wLength;		// ���������ݳ���
	BYTE	byType;			// ����������
	BYTE	byID;			// ������ID
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
// �������䳤�����ݣ��㿴һ����ô����

// CRCУ��
struct SCRCSum
{
	WORD	wCRC;
};

#pragma pack(pop)

#endif
