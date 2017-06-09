#include "stdafx.h"
#include "KDebug.h"
#include "KMemBase.h"
#include "KStrBase.h"
#include "KFilePath.h"


#ifndef WIN32
#include <unistd.h>
#else
#include <direct.h>
#endif
#include <string.h>
//---------------------------------------------------------------------------
#ifdef WIN32
static char szRootPath[MAXPATH] = "C:";		// ����·��
static char szCurrPath[MAXPATH] = "\\";		// ��ǰ·��
#else
static char szRootPath[MAXPATH] = "/";		// ����·��
static char szCurrPath[MAXPATH] = "/";		// ��ǰ·��
#endif

int RemoveTwoPointPath(LPTSTR szPath, int nLength)
{
	int nRemove = 0;
	//KASSERT(szPath);
#ifdef WIN32
	LPCTSTR lpszOld = "\\..\\";
#else
	LPCTSTR lpszOld = "/../";
#endif
	LPTSTR lpszTarget = strstr(szPath, lpszOld);
	if (lpszTarget)
	{
		LPTSTR lpszAfter = lpszTarget + 3;
		while(lpszTarget > szPath)
		{
			lpszTarget--;
			if ((*lpszTarget) == '\\' ||(*lpszTarget) == '/')
				break;
		}
		memmove(lpszTarget, lpszAfter, (nLength - (lpszAfter - szPath) + 1) * sizeof(char));
		nRemove = (lpszAfter - lpszTarget);
		return RemoveTwoPointPath(szPath, nLength - nRemove);
	}

	return nLength - nRemove;
}

int RemoveOnePointPath(LPTSTR szPath, int nLength)
{
	int nRemove = 0;
	KASSERT(szPath);
#ifdef WIN32
	LPCTSTR lpszOld = "\\.\\";
#else
	LPCTSTR lpszOld = "/./";
#endif
	LPTSTR lpszTarget = strstr(szPath, lpszOld);
	if (lpszTarget)
	{
		LPTSTR lpszAfter = lpszTarget + 2;
		memmove(lpszTarget, lpszAfter, (nLength - (lpszAfter - szPath) + 1) * sizeof(char));
		nRemove = (lpszAfter - lpszTarget);
		return RemoveOnePointPath(szPath, nLength - nRemove);
	}

	return nLength - nRemove;
}

int RemoveAllPointPath(LPTSTR szPath, int nLength)
{
	return RemoveOnePointPath(szPath, RemoveTwoPointPath(szPath, nLength));
}

//---------------------------------------------------------------------------
// ����:	SetRootPath
// ����:	���ó���ĸ�·��
// ����:	lpPathName	·����
// ����:	void
//---------------------------------------------------------------------------
void g_SetRootPath(char *pstrPathName)
{
	if (pstrPathName)
	{
		g_DebugLog("set path %s\n", pstrPathName);
		g_StrCpy(szRootPath, pstrPathName);
	}
	else
	{
		memset( szRootPath, 0, sizeof( szRootPath ) );

		getcwd( szRootPath, MAXPATH );
	}

	// ȥ��·��ĩβ�� '\'
	int len = g_StrLen(szRootPath);
	g_DebugLog("set path %s(%d)\n", szRootPath, len);
	if (szRootPath[len - 1] == '\\' || szRootPath[len - 1] == '/')
	{
		szRootPath[len - 1] = 0;
	}
	g_DebugLog("RootPath = %s", szRootPath);
}
//---------------------------------------------------------------------------
// ����:	GetRootPath
// ����:	ȡ�ó���ĸ�·��
// ����:	lpPathName	·����
// ����:	void
//---------------------------------------------------------------------------
void g_GetRootPath(char *pstrPathName)
{
	g_StrCpy(pstrPathName, szRootPath);
}
//---------------------------------------------------------------------------
// ����:	GetFullPath
// ����:	ȡ���ļ���ȫ·����
// ����:	lpPathName	·����
//			lpFileName	�ļ���
// ����:	void
//---------------------------------------------------------------------------
void g_GetFullPath(char *pstrPathName, char *pstrFileName)
{
	// �ļ�����ȫ·��
	if (pstrFileName[1] == ':')
	{
		g_StrCpy(pstrPathName, pstrFileName);
		return;
	}

	// �ļ����в���·��
	if (pstrFileName[0] == '\\' || pstrFileName[0] == '/')
	{
		g_StrCpy(pstrPathName, szRootPath);
		g_StrCat(pstrPathName, pstrFileName);
		return;
	}
	
	// ��ǰ·��Ϊȫ·��
#ifdef WIN32
	if (szCurrPath[1] == ':')
	{
		g_StrCpy(pstrPathName, szCurrPath);
		g_StrCat(pstrPathName, pstrFileName);
		return;
	}
#endif
	// ��ǰ·��Ϊ����·��
	g_StrCpy(pstrPathName, szRootPath);
        if(szCurrPath[0] != '\\' && szCurrPath[0] != '/') {
#ifdef WIN32
	g_StrCat(pstrPathName, "\\");
#else
	g_StrCat(pstrPathName, "/");
#endif
      }
	g_StrCat(pstrPathName, szCurrPath);

	if (pstrFileName[0] == '.' && (pstrFileName[1] == '\\'||pstrFileName[1] == '/') )
		g_StrCat(pstrPathName, pstrFileName + 2);
	else
		g_StrCat(pstrPathName, pstrFileName);
}
