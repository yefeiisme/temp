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
	WORD	wSceneID;			// �ֳ�ID
	BYTE	bySensorCount;	// ���ֳ�����������
	UINT	uTime;				// ʱ��
	float	fLatitude;		// �ֳ�γ��
	float	fLongitude;		// �ֳ�����
};

struct SSensorHead
{
	WORD	wLength;		// ���������ݳ���
	BYTE	byType;			// ����������
	BYTE	byID;			// �������ֳ�ID
};

struct SSensorData20		//RDK=1----Ӧ�ø�Ϊ20��
{
	double	dX;			//����
	double	dY;			//ά��
	float	fZ;			//�߳�
	BYTE	byFix;		//FIX
	BYTE	byData[1];  //����
};

struct SSensorData30		//485_��б��_s100		
{
	short	sX;			//x���
	short	sY;			//y���
	BYTE	byData[4];   //����
};

struct SSensorData31	//485_������
{
	long	lYL_Num;		//����lL
	BYTE	byData[4];  //����
};

struct SSensorData32	//485_����Һλ��
{
	short	sL;		//ҺλsL
	short	sH;		//�վ�sH
	BYTE	byData[4];  //����
};

struct SSensorData35			//485_��б��_s3300
{
	short	sX;			//x���
	short	sY;			//y���
	BYTE	byData[4];  //����
};

struct SSensorData4x	//����_��ѹ����
{
	short	sFi;			//Ƶ�ʶ���
	float	fP;				//ѹǿֵP=K*(fi*fi-fo*fo),ϵ��K ȡ��Sensor���ж�Ӧ������֮P1,Ƶ��fo=У׼ʱ��fi����У׼ʱ����Sensor֮P2,����ʱȡ��;
	BYTE	byData[2];//����
};

/*
struct SSensorData41	//����_ê���ƣ�ֱ����ʾƵ�ʣ�
{
	short	sFi;		//Ƶ�ʶ���f0
	float	fP;			//�غ�ֵP=K*(fi*fi-fo*fo),ϵ��K ȡ��Sensor���ж�Ӧ������֮P1,Ƶ��fo��=У׼ʱ��fi����У׼ʱ����Sensor֮P2,����ʱȡ��;
	BYTE	byData[2];//����
};

struct SSensorData42	//����_��ѹ��
{
	short	sFi;			//Ƶ�ʶ���
	float	fW;				//ˮλֵW=100*K*(fi*fi-fo*fo)(m),ϵ��K ȡ��Sensor���ж�Ӧ������֮P1,Ƶ��fo��=У׼ʱ��fi����У׼ʱ����Sensor֮P2,����ʱȡ��;
	BYTE	byData[2];//����
};
*/
// CRCУ��
struct SCRCSum
{
	WORD	wCRC;
};

#pragma pack(pop)

#endif
