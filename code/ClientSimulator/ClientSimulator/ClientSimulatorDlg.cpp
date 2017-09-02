
// ClientSimulatorDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ClientSimulator.h"
#include "ClientSimulatorDlg.h"
#include "afxdialogex.h"
#include "ISimulatorLogic.h"
#include "UI2LogicProtocol.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CClientSimulatorDlg �Ի���



CClientSimulatorDlg::CClientSimulatorDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CClientSimulatorDlg::IDD, pParent)
, m_strAccountMsg(_T(""))
, m_strPassword(_T(""))
, m_strSlopeID(_T(""))
, m_strSensorID(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CClientSimulatorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_MSG, m_strAccountMsg);
	DDV_MaxChars(pDX, m_strAccountMsg, 1024);
	DDX_Text(pDX, IDC_EDIT_MSG2, m_strPassword);
	DDV_MaxChars(pDX, m_strPassword, 1024);
	DDX_Text(pDX, IDC_EDIT_SLOPE_ID, m_strSlopeID);
	DDV_MaxChars(pDX, m_strSlopeID, 16);
	DDX_Text(pDX, IDC_EDIT_SENSOR_ID, m_strSensorID);
	DDV_MaxChars(pDX, m_strSensorID, 16);
}

BEGIN_MESSAGE_MAP(CClientSimulatorDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
ON_BN_CLICKED(IDOK, &CClientSimulatorDlg::OnBnClickedAppSend)
ON_BN_CLICKED(IDCANCEL, &CClientSimulatorDlg::OnBnClickedCancel)
ON_BN_CLICKED(IDOK_WEB_SEND, &CClientSimulatorDlg::OnBnClickedWebSend)
ON_BN_CLICKED(IDC_BUTTON_SLOPE, &CClientSimulatorDlg::OnBnClickedButtonSlope)
ON_BN_CLICKED(IDC_BUTTON_SENSOR_LIST, &CClientSimulatorDlg::OnBnClickedButtonSensorList)
ON_BN_CLICKED(IDC_BUTTON_SENSOR_HISTORY, &CClientSimulatorDlg::OnBnClickedButtonSensorHistory)
END_MESSAGE_MAP()


// CClientSimulatorDlg ��Ϣ�������

BOOL CClientSimulatorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CClientSimulatorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CClientSimulatorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CClientSimulatorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



BOOL CClientSimulatorDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;

	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
		return TRUE;

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CClientSimulatorDlg::OnBnClickedAppSend()
{
	UpdateData(TRUE);

	U2L_APP_LOGIN	tagAppLogin;
	memset(&tagAppLogin, 0, sizeof(tagAppLogin));
	tagAppLogin.byProtocol	= u2l_app_login;
	strncpy(tagAppLogin.strAccount, m_strAccountMsg.GetBuffer(m_strAccountMsg.GetLength()), sizeof(tagAppLogin.strAccount));
	tagAppLogin.strAccount[sizeof(tagAppLogin.strAccount)-1]	= '\0';
	strncpy(tagAppLogin.strPassword, m_strPassword.GetBuffer(m_strPassword.GetLength()), sizeof(tagAppLogin.strPassword));
	tagAppLogin.strPassword[sizeof(tagAppLogin.strPassword) - 1]	= '\0';

	g_ISimulatorLogic.SendRequest(&tagAppLogin, sizeof(tagAppLogin));
}

void CClientSimulatorDlg::OnBnClickedWebSend()
{
	UpdateData(TRUE);

	U2L_WEB_LOGIN	tagWebLogin;
	memset(&tagWebLogin, 0, sizeof(tagWebLogin));
	tagWebLogin.byProtocol	= u2l_web_login;
	strncpy(tagWebLogin.strAccount, m_strAccountMsg.GetBuffer(m_strAccountMsg.GetLength()), sizeof(tagWebLogin.strAccount));
	tagWebLogin.strAccount[sizeof(tagWebLogin.strAccount) - 1]	= '\0';
	strncpy(tagWebLogin.strPassword, m_strPassword.GetBuffer(m_strPassword.GetLength()), sizeof(tagWebLogin.strPassword));
	tagWebLogin.strPassword[sizeof(tagWebLogin.strPassword) - 1]	= '\0';

	g_ISimulatorLogic.SendRequest(&tagWebLogin, sizeof(tagWebLogin));
}


void CClientSimulatorDlg::OnBnClickedCancel()
{
	UpdateData(TRUE);

	CDialogEx::OnCancel();
}



void CClientSimulatorDlg::OnBnClickedButtonSlope()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}


void CClientSimulatorDlg::OnBnClickedButtonSensorList()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}


void CClientSimulatorDlg::OnBnClickedButtonSensorHistory()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}
