#include "stdafx.h"
#include "KFile.h"
#include "KMemBase.h"
#include "KDebug.h"
#include <string.h>

//---------------------------------------------------------------------------
// ����:	g_DebugLog
// ����:	����Դ������������Ϣ
// ����:	Fmt		��ʽ���ַ���
//			...		������ַ���
// ����:	void
//---------------------------------------------------------------------------
void g_DebugLog(LPSTR Fmt, ...)
{
}
//---------------------------------------------------------------------------
// ����:	g_MessageBox
// ����:	Display a System Message Box
// ����:	char* lpMsg, ...
// ����:	void
//---------------------------------------------------------------------------
void g_MessageBox(LPSTR lpMsg, ...)
{
}
//---------------------------------------------------------------------------
// ����:	g_AssertFailed
// ����:	����ʧ��
// ����:	FileName	���ĸ��ļ���ʧ��
//			LineNum		���ļ��еĵڼ���
// ����:	void
//---------------------------------------------------------------------------
void g_AssertFailed(LPSTR pFileName, int nLineNum)
{
	char szMsg[256];
	sprintf(szMsg, "Assert failed in %s, line = %i", pFileName, nLineNum);
	g_DebugLog(szMsg);
	g_MessageBox(szMsg);
	exit(1);
}
