
// ClientSimulatorDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ClientSimulator.h"
#include "ClientSimulatorDlg.h"
#include "afxdialogex.h"
#include "ISimulatorLogic.h"
#include "UI2LogicProtocol.h"
#include "iconv.h"

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
, m_wSlopeID(0)
, m_uSensorID(0)
, m_wServerID(0)
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
	DDX_Text(pDX, IDC_EDIT_SLOPE_ID, m_wSlopeID);
	DDX_Text(pDX, IDC_EDIT_SENSOR_ID, m_uSensorID);
	DDX_Text(pDX, IDC_EDIT_SERVER_ID, m_wServerID);
}

BEGIN_MESSAGE_MAP(CClientSimulatorDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
ON_BN_CLICKED(IDOK, &CClientSimulatorDlg::OnBnClickedAppSend)
ON_BN_CLICKED(IDOK_WEB_SEND, &CClientSimulatorDlg::OnBnClickedWebSend)
ON_BN_CLICKED(IDC_BUTTON_ASLOPE, &CClientSimulatorDlg::OnBnClickedButtonAslope)
ON_BN_CLICKED(IDC_BUTTON_ASENSOR_LIST, &CClientSimulatorDlg::OnBnClickedButtonAsensorList)
ON_BN_CLICKED(IDC_BUTTON_ASENSOR_HISTORY, &CClientSimulatorDlg::OnBnClickedButtonAsensorHistory)
ON_BN_CLICKED(IDC_BUTTON_WSLOPE, &CClientSimulatorDlg::OnBnClickedButtonWslope)
ON_BN_CLICKED(IDC_BUTTON_WSENSOR_LIST, &CClientSimulatorDlg::OnBnClickedButtonWsensorList)
ON_BN_CLICKED(IDC_BUTTON_WSENSOR_HISTORY, &CClientSimulatorDlg::OnBnClickedButtonWsensorHistory)
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

	char szPlayerName[0xffff];
	memset(szPlayerName, 0, sizeof(szPlayerName));

	size_t		nAccountLen		= m_strAccountMsg.GetLength();
	size_t		nOutAccountLen	= sizeof(tagAppLogin.strAccount);

	const char	*pstrAccount	= m_strAccountMsg.GetBuffer(m_strAccountMsg.GetLength());
	char		*pstrOutAccount = tagAppLogin.strAccount;

	size_t		nPasswordLen	= m_strPassword.GetLength();
	size_t		nOutPasswordLen	= sizeof(tagAppLogin.strPassword);

	const char	*pstrPassword	= m_strPassword.GetBuffer(m_strPassword.GetLength());
	char		*pstrOutPassword= tagAppLogin.strPassword;

	iconv_t pLibiconv = iconv_open("gb2312", "utf-8");

	if (-1 == iconv(pLibiconv, &pstrAccount, &nAccountLen, &pstrOutAccount, &nOutAccountLen))
	{
		iconv_close(pLibiconv);
		AfxMessageBox("[%s][%d] iconv gb2312 to utf-8 Failed\n", MB_OK);
		return;
	}

	if (-1 == iconv(pLibiconv, &pstrPassword, &nPasswordLen, &pstrOutPassword, &nOutPasswordLen))
	{
		iconv_close(pLibiconv);
		AfxMessageBox("[%s][%d] iconv gb2312 to utf-8 Failed\n", MB_OK);
		return;
	}

	iconv_close(pLibiconv);

	//strncpy(tagAppLogin.strAccount, m_strAccountMsg.GetBuffer(m_strAccountMsg.GetLength()), sizeof(tagAppLogin.strAccount));
	//tagAppLogin.strAccount[sizeof(tagAppLogin.strAccount)-1]	= '\0';
	//strncpy(tagAppLogin.strPassword, m_strPassword.GetBuffer(m_strPassword.GetLength()), sizeof(tagAppLogin.strPassword));
	//tagAppLogin.strPassword[sizeof(tagAppLogin.strPassword) - 1]	= '\0';

	g_ISimulatorLogic.SendRequest(&tagAppLogin, sizeof(tagAppLogin));
}

void CClientSimulatorDlg::OnBnClickedButtonAslope()
{
	UpdateData(TRUE);

	U2L_APP_SLOPE_LIST	tagAppSlopeList;
	memset(&tagAppSlopeList, 0, sizeof(tagAppSlopeList));
	tagAppSlopeList.byProtocol	= u2l_app_slope_list;
	tagAppSlopeList.wServerID	= m_wServerID;

	g_ISimulatorLogic.SendRequest(&tagAppSlopeList, sizeof(tagAppSlopeList));
}

void CClientSimulatorDlg::OnBnClickedButtonAsensorList()
{
	UpdateData(TRUE);

	U2L_APP_SENSOR_LIST	tagAppSensorList;
	memset(&tagAppSensorList, 0, sizeof(tagAppSensorList));
	tagAppSensorList.byProtocol	= u2l_app_sensor_list;
	tagAppSensorList.wSlopeID	= m_wSlopeID;

	g_ISimulatorLogic.SendRequest(&tagAppSensorList, sizeof(tagAppSensorList));
}

void CClientSimulatorDlg::OnBnClickedButtonAsensorHistory()
{
	UpdateData(TRUE);

	U2L_APP_SENSOR_HISTORY	tagAppSensorHistory;
	memset(&tagAppSensorHistory, 0, sizeof(tagAppSensorHistory));
	tagAppSensorHistory.byProtocol	= u2l_app_sensor_history;
	tagAppSensorHistory.uSensorID	= m_uSensorID;

	g_ISimulatorLogic.SendRequest(&tagAppSensorHistory, sizeof(tagAppSensorHistory));
}

void CClientSimulatorDlg::OnBnClickedWebSend()
{
	UpdateData(TRUE);

	U2L_WEB_LOGIN	tagWebLogin;
	memset(&tagWebLogin, 0, sizeof(tagWebLogin));
	tagWebLogin.byProtocol = u2l_web_login;
	strncpy(tagWebLogin.strAccount, m_strAccountMsg.GetBuffer(m_strAccountMsg.GetLength()), sizeof(tagWebLogin.strAccount));
	tagWebLogin.strAccount[sizeof(tagWebLogin.strAccount) - 1] = '\0';
	strncpy(tagWebLogin.strPassword, m_strPassword.GetBuffer(m_strPassword.GetLength()), sizeof(tagWebLogin.strPassword));
	tagWebLogin.strPassword[sizeof(tagWebLogin.strPassword) - 1] = '\0';

	g_ISimulatorLogic.SendRequest(&tagWebLogin, sizeof(tagWebLogin));
}

void CClientSimulatorDlg::OnBnClickedButtonWslope()
{
	UpdateData(TRUE);

	U2L_WEB_SLOPE_LIST	tagWebSlopeList;
	memset(&tagWebSlopeList, 0, sizeof(tagWebSlopeList));
	tagWebSlopeList.byProtocol	= u2l_web_slope_list;
	tagWebSlopeList.wServerID	= m_wServerID;

	g_ISimulatorLogic.SendRequest(&tagWebSlopeList, sizeof(tagWebSlopeList));
}

void CClientSimulatorDlg::OnBnClickedButtonWsensorList()
{
	UpdateData(TRUE);

	U2L_WEB_SENSOR_LIST	tagWebSensorList;
	memset(&tagWebSensorList, 0, sizeof(tagWebSensorList));
	tagWebSensorList.byProtocol	= u2l_web_sensor_list;
	tagWebSensorList.wSlopeID	= m_wSlopeID;

	g_ISimulatorLogic.SendRequest(&tagWebSensorList, sizeof(tagWebSensorList));
}

void CClientSimulatorDlg::OnBnClickedButtonWsensorHistory()
{
	UpdateData(TRUE);

	U2L_WEB_SENSOR_HISTORY	tagWebSensorHistory;
	memset(&tagWebSensorHistory, 0, sizeof(tagWebSensorHistory));
	tagWebSensorHistory.byProtocol	= u2l_web_sensor_history;
	tagWebSensorHistory.uSensorID	= m_uSensorID;

	g_ISimulatorLogic.SendRequest(&tagWebSensorHistory, sizeof(tagWebSensorHistory));
}
