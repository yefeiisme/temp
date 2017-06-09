#include "stdafx.h"
#include "KDebug.h"
#include "KMemBase.h"
#include "MemClass.h"
#include "KMemClass1.h"

CMemClass::CMemClass()
{
	m_lpMemPtr = NULL;
	m_lpMemLen = 0;
}

CMemClass::~CMemClass()
{
	Free();
}
//---------------------------------------------------------------------------
// ����:	Alloc
// ����:	�����ڴ�
// ����:	dwSize		�ڴ���С
// ����:	PVOID		�ڴ��ָ��
//---------------------------------------------------------------------------
void *CMemClass::Alloc(unsigned int uiSize)
{
	// �Ѿ�����ľͲ�Ҫ�ٷ�����
	if (m_lpMemLen == uiSize)
		return m_lpMemPtr;

	// �ͷ��Ѿ�������ڴ�
	if (m_lpMemPtr)
		Free();

	// �����ڴ�
	m_lpMemPtr = g_MemAlloc(uiSize);
	if (m_lpMemPtr != NULL)
		m_lpMemLen = uiSize;
	return m_lpMemPtr;
}

//---------------------------------------------------------------------------
// ����:	Free
// ����:	�ͷ��ڴ�
// ����:	void
// ����:	void
//---------------------------------------------------------------------------
void CMemClass::Free()
{
	if (m_lpMemPtr)
		g_MemFree(m_lpMemPtr);
	m_lpMemPtr = NULL;
	m_lpMemLen = 0;
}

void *KMemClass1::Alloc(DWORD dwSize)
{
	this->m_lpMemPtr = new BYTE[dwSize];
	this->m_lpMemLen = dwSize;
	return m_lpMemPtr;
}

void KMemClass1::Free()
{
	delete []this->m_lpMemPtr;
}
