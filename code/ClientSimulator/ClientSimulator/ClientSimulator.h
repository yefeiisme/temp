
// ClientSimulator.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CClientSimulatorApp: 
// �йش����ʵ�֣������ ClientSimulator.cpp
//

class CClientSimulatorApp : public CWinApp
{
public:
	CClientSimulatorApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CClientSimulatorApp theApp;