
// ClientSimulatorDlg.cpp : 实现文件
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


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CClientSimulatorDlg 对话框



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


// CClientSimulatorDlg 消息处理程序

BOOL CClientSimulatorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CClientSimulatorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
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
	// TODO:  在此添加控件通知处理程序代码
}


void CClientSimulatorDlg::OnBnClickedButtonSensorList()
{
	// TODO:  在此添加控件通知处理程序代码
}


void CClientSimulatorDlg::OnBnClickedButtonSensorHistory()
{
	// TODO:  在此添加控件通知处理程序代码
}
