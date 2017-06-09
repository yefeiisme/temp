#include "stdafx.h"
#include "KDebug.h"
#include "KNode.h"
#include "KList.h"
#include "KMemBase.h"
#include <string.h>
//---------------------------------------------------------------------------
class KMemNode : public KNode
{
public:
	DWORD	m_dwMemSize;//�ڴ��С
	DWORD	m_dwMemSign;//�ڴ��־
};
//---------------------------------------------------------------------------
class KMemList : public KList
{
public:
	~KMemList()
	{
		KMemNode* pNode = (KMemNode*)GetHead();
		while (pNode)
		{
			g_DebugLog("KMemList::Leak Detected, Size = %d", pNode->m_dwMemSize);
			pNode = (KMemNode*)pNode->GetNext();
		}
	};
	void ShowUsage()
	{
		KMemNode* pNode = (KMemNode*)GetHead();
		DWORD dwMemSize = 0;
		while (pNode)
		{
			dwMemSize += pNode->m_dwMemSize;
			pNode = (KMemNode*)pNode->GetNext();
		}
		g_DebugLog("Memory Usage Size = %d KB", dwMemSize >> 10 );
	}
};
static KMemList m_MemList;
//---------------------------------------------------------------------------
#define MEMSIGN 1234567890
//---------------------------------------------------------------------------
// ����:	g_MemAlloc
// ����:	�����ڴ�
// ����:	dwSize		�ڴ���С
// ����:	lpMem (lpMem = NULL ��ʾ����ʧ��)
//---------------------------------------------------------------------------
 LPVOID g_MemAlloc(DWORD dwSize)
{
//	HANDLE hHeap = GetProcessHeap();
	PBYTE  lpMem = NULL;
	DWORD  dwHeapSize = dwSize + sizeof(KMemNode);

//	lpMem = (PBYTE)HeapAlloc(hHeap, 0, dwHeapSize);
	lpMem = (PBYTE)new char[dwHeapSize];
	if (NULL == lpMem)
	{
		g_MessageBox("g_MemAlloc() Failed, Size = %d", dwSize);
		return NULL;
	}

	KMemNode* pNode = (KMemNode*)lpMem;
	pNode->m_pPrev = NULL;
	pNode->m_pNext = NULL;
	pNode->m_dwMemSize = dwSize;
	pNode->m_dwMemSign = MEMSIGN;
	m_MemList.AddHead(pNode);
	
	return (lpMem + sizeof(KMemNode));
//	return 0;
}
//---------------------------------------------------------------------------
// ����:	g_MemFree
// ����:	�ͷ��ڴ�
// ����:	lpMem		Ҫ�ͷŵ��ڴ�ָ��
// ����:	void
//---------------------------------------------------------------------------
 void g_MemFree(LPVOID lpMem)
{
//	HANDLE hHeap = GetProcessHeap();
	if (lpMem == NULL)
		return;
	lpMem = (PBYTE)lpMem - sizeof(KMemNode);
	KMemNode* pNode = (KMemNode *)lpMem;
	if (pNode->m_dwMemSign != MEMSIGN)
	{
		g_MessageBox("g_MemFree() Failed, Size = %d", pNode->m_dwMemSize);
		return;
	}
	pNode->Remove();
//	HeapFree(hHeap, 0, lpMem);
	delete[] lpMem;
}
//---------------------------------------------------------------------------
// ����:	MemoryCopy
// ����:	�ڴ濽��
// ����:	lpDest	:	Ŀ���ڴ��
//			lpSrc	:	Դ�ڴ��
//			dwLen	:	��������
// ����:	void
//---------------------------------------------------------------------------
 void g_MemCopy(PVOID lpDest, PVOID lpSrc, DWORD dwLen)
{	
#ifdef WIN32
	__asm
	{
		mov		edi, lpDest
		mov		esi, lpSrc
		mov		ecx, dwLen
		mov     ebx, ecx
		shr     ecx, 2
		rep     movsd
		mov     ecx, ebx
		and     ecx, 3
		rep     movsb
	}
#else
     memcpy(lpDest, lpSrc, dwLen);
#endif
}
//---------------------------------------------------------------------------
// ����:	MemoryCopyMmx
// ����:	�ڴ濽����MMX�汾��
// ����:	lpDest	:	Ŀ���ڴ��
//			lpSrc	:	Դ�ڴ��
//			dwLen	:	��������
// ����:	void
//---------------------------------------------------------------------------
 void g_MemCopyMmx(PVOID lpDest, PVOID lpSrc, DWORD dwLen)
{
#ifdef WIN32
	__asm
	{
		mov		edi, lpDest
		mov		esi, lpSrc
		mov		ecx, dwLen
		mov     ebx, ecx
		shr     ecx, 3
		jcxz	loc_copy_mmx2

loc_copy_mmx1:

		movq	mm0, [esi]
		add		esi, 8
		movq	[edi], mm0
		add		edi, 8
		dec		ecx
		jnz		loc_copy_mmx1

loc_copy_mmx2:

		mov     ecx, ebx
		and     ecx, 7
		rep     movsb
		emms
	}
#else
     memcpy(lpDest, lpSrc, dwLen);
#endif
}
//---------------------------------------------------------------------------
// ����:	MemoryComp
// ����:	�ڴ�Ƚ�
// ����:	lpDest	:	�ڴ��1
//			lpSrc	:	�ڴ��2
//			dwLen	:	�Ƚϳ���
// ����:	TRUE	:	��ͬ
//			FALSE	:	��ͬ	
//---------------------------------------------------------------------------
 BOOL g_MemComp(PVOID lpDest, PVOID lpSrc, DWORD dwLen)
{	
#ifdef WIN32
	__asm
	{
		mov		edi, lpDest
		mov		esi, lpSrc
		mov		ecx, dwLen
		mov     ebx, ecx
		shr     ecx, 2
		rep     cmpsd
		jne		loc_not_equal
		mov     ecx, ebx
		and     ecx, 3
		rep     cmpsb
		jne		loc_not_equal
	};
	return TRUE;

loc_not_equal:

	return FALSE;
#else
     return (0 == memcmp(lpDest, lpSrc, dwLen));
#endif
}
//---------------------------------------------------------------------------
// ����:	MemoryZero
// ����:	�ڴ�����
// ����:	lpDest	:	�ڴ��ַ
//			dwLen	:	�ڴ泤��
// ����:	void
//---------------------------------------------------------------------------
 void g_MemZero(PVOID lpDest, DWORD dwLen)
{
#ifdef WIN32
	__asm
	{
		mov		ecx, dwLen
		mov		edi, lpDest
		xor     eax, eax
		mov		ebx, ecx
		shr		ecx, 2
		rep     stosd
		mov     ecx, ebx
		and		ecx, 3
		rep     stosb
	}
#else
     memset(lpDest, 0, dwLen);
#endif
}
