#include "stdafx.h"
#include "KDebug.h"
#include "KStrBase.h"
#include "KFile.h"
#include "KFilePath.h"
#include "KPakFile.h"
#include "KTabFile.h"
#include <string.h>

KTabFile::KTabFile()
{
	m_Width		= 0;
	m_Height	= 0;
}

KTabFile::~KTabFile()
{
	Clear();
}
//---------------------------------------------------------------------------
// ����:	Load
// ����:	����һ��Tab�ļ�
// ����:	FileName	�ļ���
// ����:	true		�ɹ�
//			false		ʧ��
//---------------------------------------------------------------------------
bool KTabFile::Load(const char *FileName)
{
	KPakFile	File;
	DWORD		dwSize;
	PVOID		Buffer;

	// check file name
	if (FileName[0] == 0)
		return false;

	if (!File.Open(FileName))
	{
		g_DebugLog("Can't open tab file : %s", FileName);
		return false;
	}

	dwSize = File.Size();

	Buffer = m_Memory.Alloc(dwSize);

	File.Read(Buffer, dwSize);
	
	if (dwSize)
		CreateTabOffset();
	else
		return false;

	return true;
}

//---------------------------------------------------------------------------
// ����:	CreateTabOffset
// ����:	������������ָ��ļ���ƫ�Ʊ�
// ����:	void
// ����:	void
//---------------------------------------------------------------------------
void KTabFile::CreateTabOffset()
{
	int		nWidth, nHeight, nOffset, nSize;
	BYTE	*Buffer;
	TABOFFSET *TabBuffer;

	nWidth	= 1;
	nHeight	= 1;
	nOffset = 0;

	Buffer	= (BYTE *)m_Memory.GetMemPtr();
	nSize	= m_Memory.GetMemLen();
	
	if (!Buffer || !nSize)
		return;

	// ����һ�о����ж�����
	while (*Buffer != 0x0d && *Buffer != 0x0a)
	{
		if (*Buffer == 0x09)
		{
			nWidth++;
		}
		Buffer++;
		nOffset++;
	}
	if (*Buffer == 0x0d && *(Buffer + 1) == 0x0a)
	{
		Buffer += 2;	// 0x0a����		
		nOffset += 2;	// 0x0a����
	}
	else
	{
		Buffer += 1;	// 0x0a����		
		nOffset += 1;	// 0x0a����
	}
	while(nOffset < nSize)
	{
		while (*Buffer != 0x0d && *Buffer != 0x0a)
		{
			Buffer++;
			nOffset++;
			if (nOffset >= nSize)
				break;
		}
		nHeight++;
		if (*Buffer == 0x0d && *(Buffer + 1) == 0x0a)
		{
			Buffer += 2;	// 0x0a����		
			nOffset += 2;	// 0x0a����
		}
		else
		{
			Buffer += 1;	// 0x0a����		
			nOffset += 1;	// 0x0a����
		}
	}
	m_Width		= nWidth;
	m_Height	= nHeight;

	TabBuffer = (TABOFFSET *)m_OffsetTable.Alloc(m_Width * m_Height * sizeof (TABOFFSET));
	Buffer = (BYTE *)m_Memory.GetMemPtr();

	nOffset = 0;
	int nLength;
	for (int i = 0; i < nHeight; i++)
	{
		for (int j = 0; j < nWidth; j++)
		{
			TabBuffer->dwOffset = nOffset;	
			nLength = 0;
			while(*Buffer != 0x09 && *Buffer != 0x0d && *Buffer != 0x0a && nOffset < nSize)
			{
				Buffer++;
				nOffset++;
				nLength++;
			}
			Buffer++;	// 0x09��0x0d��0x0a(linux)����
			nOffset++;
			TabBuffer->dwLength = nLength;
			TabBuffer++;
			if (*(Buffer - 1) == 0x0a || *(Buffer - 1) == 0x0d)	//	�����Ѿ������ˣ���Ȼ����û��nWidth //for linux modified [wxb 2003-7-29]
			{
				for (int k = j+1; k < nWidth; k++)
				{
					TabBuffer->dwOffset = nOffset;
					TabBuffer->dwLength = 0;
					TabBuffer++;					
				}
				break;
			}
		}

		//modified for linux [wxb 2003-7-29]
		if (*(Buffer - 1) == 0x0d && *Buffer == 0x0a)
		{
			Buffer++;				// 0x0a����	
			nOffset++;				// 0x0a����	
		}
	}
}

//---------------------------------------------------------------------------
// ����:	GetString
// ����:	ȡ��ĳ��ĳ���ַ�����ֵ
// ����:	nRow			��
//			nColomn			��
//			lpDefault		ȱʡֵ
//			lpRString		����ֵ
//			dwSize			�����ַ�������󳤶�
// ����:	�Ƿ�ɹ�
//---------------------------------------------------------------------------
bool KTabFile::GetString(const int nRow, char *pstrColumn, char *pstrDefault, char *pstrRString, const DWORD dwSize)
{
	int nColumn = FindColumn(pstrColumn);

	if (GetValue(nRow - 1, nColumn - 1, pstrRString, dwSize))
		return true;

	g_StrCpyLen(pstrRString, pstrDefault, dwSize);

	return false;
}
//---------------------------------------------------------------------------
// ����:	GetString
// ����:	ȡ��ĳ��ĳ���ַ�����ֵ
// ����:	nRow			��		��1��ʼ
//			nColomn			��		��1��ʼ
//			lpDefault		ȱʡֵ
//			lpRString		����ֵ
//			dwSize			�����ַ�������󳤶�
// ����:	�Ƿ�ɹ�
//---------------------------------------------------------------------------
bool KTabFile::GetString(const int nRow, const int nColumn, char *lpDefault, char *lpRString, const DWORD dwSize)
{
	if (GetValue(nRow - 1, nColumn - 1,  lpRString, dwSize))
		return true;
	g_StrCpyLen(lpRString, lpDefault, dwSize);
	return false;
}
//---------------------------------------------------------------------------
// ����:	GetInteger
// ����:	ȡ��ĳ��ĳ���ַ�����ֵ
// ����:	nRow			��
//			szColomn		��
//			nDefault		ȱʡֵ
//			pnValue			����ֵ
// ����:	�Ƿ�ɹ�
//---------------------------------------------------------------------------
bool KTabFile::GetInteger(const int nRow, char *szColumn, const int nDefault, int *pnValue)
{
	char	Buffer[32];
	int		nColumn = FindColumn(szColumn);

	if (GetValue(nRow - 1, nColumn - 1, Buffer, sizeof(Buffer)))
	{
		*pnValue = atoi(Buffer);
		return true;
	}
	else
	{
		*pnValue = nDefault;
		return false;
	}
}
//---------------------------------------------------------------------------
// ����:	GetInteger
// ����:	ȡ��ĳ��ĳ���ַ�����ֵ
// ����:	nRow			��		��1��ʼ
//			nColomn			��		��1��ʼ
//			nDefault		ȱʡֵ
//			pnValue			����ֵ
// ����:	�Ƿ�ɹ�
//---------------------------------------------------------------------------
bool KTabFile::GetInteger(const int nRow, const int nColumn, const int nDefault, int *pnValue)
{
	char	Buffer[32];

	if (GetValue(nRow - 1, nColumn - 1, Buffer, sizeof(Buffer)))
	{
		*pnValue = atoi(Buffer);
		return true;
	}
	else
	{
		*pnValue = nDefault;
		return true;
	}
}
//---------------------------------------------------------------------------
// ����:	GetFloat
// ����:	ȡ��ĳ��ĳ���ַ�����ֵ
// ����:	nRow			��
//			szColomn		��
//			nDefault		ȱʡֵ
//			pnValue			����ֵ
// ����:	�Ƿ�ɹ�
//---------------------------------------------------------------------------
bool KTabFile::GetFloat(const int nRow, char *szColumn, const float fDefault, float *pfValue)
{
	char	Buffer[32];
	int		nColumn = FindColumn(szColumn);

	if (GetValue(nRow - 1, nColumn - 1, Buffer, sizeof(Buffer)))
	{
		*pfValue = (float)atof(Buffer);
		return true;
	}
	else
	{
		*pfValue = fDefault;
		return false;
	}
}
//---------------------------------------------------------------------------
// ����:	GetFloat
// ����:	ȡ��ĳ��ĳ���ַ�����ֵ
// ����:	nRow			��		��1��ʼ
//			nColomn			��		��1��ʼ
//			nDefault		ȱʡֵ
//			pnValue			����ֵ
// ����:	�Ƿ�ɹ�
//---------------------------------------------------------------------------
bool KTabFile::GetFloat(const int nRow, const int nColumn, const float fDefault, float *pfValue)
{
	char	Buffer[32];
	
	if (GetValue(nRow - 1, nColumn - 1, Buffer, sizeof(Buffer)))
	{
		*pfValue = (float)atof(Buffer);
		return true;
	}
	else
	{
		*pfValue = fDefault;
		return false;
	}
}
//---------------------------------------------------------------------------
// ����:	GetValue
// ����:	ȡ��ĳ��ĳ���ַ�����ֵ
// ����:	nRow			��
//			nColomn			��
//			lpDefault		ȱʡֵ
//			lpRString		����ֵ
//			dwSize			�����ַ�������󳤶�
// ����:	�Ƿ�ɹ�
//---------------------------------------------------------------------------
bool KTabFile::GetValue(const int nRow, const int nColumn, char *lpRString, const DWORD dwSize)
{
	if (nRow >= m_Height || nColumn >= m_Width || nRow < 0 || nColumn < 0)
		return false;

	TABOFFSET	*TempOffset;
	char		*Buffer;

	Buffer = (char*)m_Memory.GetMemPtr();
	TempOffset = (TABOFFSET *)m_OffsetTable.GetMemPtr();
	TempOffset += nRow * m_Width + nColumn;

	ZeroMemory(lpRString, dwSize);
	Buffer += TempOffset->dwOffset;
	if (TempOffset->dwLength == 0)
	{
		return false;
	}
	if (dwSize > TempOffset->dwLength)
	{
		memcpy(lpRString, Buffer, TempOffset->dwLength);
		lpRString[TempOffset->dwLength] = 0;
	}
	else
	{
		memcpy(lpRString, Buffer, dwSize);
		lpRString[dwSize] = 0;
	}

	return true;
}
//---------------------------------------------------------------------------
// ����:	Clear
// ����:	���TAB�ļ�������
// ����:	void
// ����:	void
//---------------------------------------------------------------------------
void KTabFile::Clear()
{
	m_Memory.Free();
	m_OffsetTable.Free();
}
//---------------------------------------------------------------------------
// ����:	FindRow
// ����:	�����йؼ���
// ����:	szRow���йؼ��֣�
// ����:	int
//---------------------------------------------------------------------------
int KTabFile::FindRow(char *szRow)
{
	char	szTemp[128];
	for (int i = 0; i < m_Height; i++)	// ��1��ʼ��������һ�е��ֶ���
	{
		GetValue(i, 0, szTemp, sizeof(szTemp));
		if (g_StrCmp(szTemp, szRow))
			return i + 1; //�Ķ��˴�Ϊ��һ by Romandou,��������1Ϊ���ı��
	}
	return -1;
}
//---------------------------------------------------------------------------
// ����:	FindColumn
// ����:	�����йؼ���
// ����:	szColumn���йؼ��֣�
// ����:	int
//---------------------------------------------------------------------------
int KTabFile::FindColumn(char *szColumn)
{
	char	szTemp[128];
	for (int i = 0; i < m_Width; i++)	// ��1��ʼ��������һ�е��ֶ���
	{
		GetValue(0, i, szTemp, sizeof(szTemp));
		if (g_StrCmp(szTemp, szColumn))
			return i + 1;//�Ķ��˴�Ϊ��һ by Romandou,��������1Ϊ���ı��
	}
	return -1;
}

void KTabFile::Release()
{
	Clear();
	delete this;
}

ITabFile *OpenTabFile(const char *FileName)
{
	KTabFile	*pTabFile	= new KTabFile;
	if (!pTabFile)
		return NULL;

	if (!pTabFile->Load(FileName))
	{
		pTabFile->Clear();
		delete pTabFile;
		return NULL;
	}

	return pTabFile;
}
