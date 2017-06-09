#include "stdafx.h"
#include "KFilePath.h"
#include "KPakFile.h"

KPakFile::KPakFile()
{
}

KPakFile::~KPakFile()
{
	Close();
}

//---------------------------------------------------------------------------
// ����:	��һ���ļ�, ��Ѱ�ҵ�ǰĿ¼���Ƿ���ͬ���ĵ����ļ�,
// ����:	FileName	�ļ���
// ����:	TRUE		�ɹ�
//			FALSE		ʧ��
//---------------------------------------------------------------------------

#include "string.h"

bool KPakFile::Open(const char* pszFileName)
{
	if (pszFileName == NULL || pszFileName[0] == 0)
		return false;

	bool bOk = false;
	Close();

	char szPackNameAddtion[256];

#ifdef WIN32
	int  len=strlen(pszFileName);
	int  iDestIndex=0;
	
	for (int i=0;i<len;i++)
	{
		if (pszFileName[i]!='/')
		{ 
			if (pszFileName[i]=='\\')
			{
				if (pszFileName[i+1]!='\\')
				{
					szPackNameAddtion[iDestIndex]=pszFileName[i];
					iDestIndex++;
				}//endif
				
			}//endif
			else
			{
				szPackNameAddtion[iDestIndex]=pszFileName[i];
				iDestIndex++;
			}//endelse
		}//endif
		else
		{
			szPackNameAddtion[iDestIndex]='\\';
			iDestIndex++;
		}//end else
		
	}//end for i

	szPackNameAddtion[iDestIndex]=0;
#else
	strcpy(szPackNameAddtion, pszFileName);
#endif

	bOk = (m_File.Open((char*)szPackNameAddtion) != FALSE);
	
	return bOk;
}

//---------------------------------------------------------------------------
// ����:	���ļ��ж�ȡ����
// ����:	pBuffer		������ָ��
//			dwSize		Ҫ��ȡ�ĳ���
// ����:	�������ֽڳ���
//---------------------------------------------------------------------------
unsigned int KPakFile::Read(void* pBuffer, unsigned int uSize)
{
	uSize = m_File.Read(pBuffer, uSize);

	return uSize;
}

//---------------------------------------------------------------------------
// ����:	�����ļ���С
// ����:	�ļ��Ĵ�С in bytes
//---------------------------------------------------------------------------
unsigned int KPakFile::Size()
{
	unsigned int uSize;

	uSize = m_File.Size();
	return uSize;
}
//---------------------------------------------------------------------------
// ����:	�ر�һ���ļ�
//---------------------------------------------------------------------------
void KPakFile::Close()
{
	m_File.Close();
}

void KPakFile::Release()
{
	Close();
	delete this;
}
