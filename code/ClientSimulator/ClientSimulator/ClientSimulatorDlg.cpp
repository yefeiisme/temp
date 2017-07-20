
// ClientSimulatorDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ClientSimulator.h"
#include "ClientSimulatorDlg.h"
#include "afxdialogex.h"
#include "ClientGSProtocol.h"
#include "ISimulatorLogic.h"
#include "ClientGSProtocol.pb.h"

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
, m_strSendMsg(_T(""))
, m_strPassword(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CClientSimulatorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_MSG, m_strSendMsg);
	DDV_MaxChars(pDX, m_strSendMsg, 1024);
	DDX_Text(pDX, IDC_EDIT_MSG2, m_strPassword);
	DDV_MaxChars(pDX, m_strPassword, 1024);
}

BEGIN_MESSAGE_MAP(CClientSimulatorDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
ON_BN_CLICKED(IDOK, &CClientSimulatorDlg::OnBnClickedOk)
ON_BN_CLICKED(IDCANCEL, &CClientSimulatorDlg::OnBnClickedCancel)
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


void CClientSimulatorDlg::OnBnClickedOk()
{
	UpdateData(TRUE);

	WS_CS_NET_PACK::WS2CS_LOGIN	tagSendMsg;
	BYTE	strBuffer[0xffff]	= {0};
	strBuffer[0]	= c2s_broad_cast_msg;
	tagSendMsg.set_account(m_strSendMsg.GetBuffer(m_strSendMsg.GetLength()));
	tagSendMsg.set_password(m_strPassword.GetBuffer(m_strPassword.GetLength()));
	tagSendMsg.SerializeToArray(strBuffer + sizeof(BYTE), tagSendMsg.ByteSize());

	g_ISimulatorLogic.SendRequest(strBuffer, sizeof(BYTE)+tagSendMsg.ByteSize());
}


void CClientSimulatorDlg::OnBnClickedCancel()
{
	UpdateData(TRUE);

	CDialogEx::OnCancel();
}
