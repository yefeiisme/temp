
// ClientSimulatorDlg.h : 头文件
//

#pragma once


// CClientSimulatorDlg 对话框
class CClientSimulatorDlg : public CDialogEx
{
// 构造
public:
	CClientSimulatorDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_CLIENTSIMULATOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedAppSend();
	afx_msg void OnBnClickedCancel();
	CString m_strAccountMsg;
	CString m_strPassword;
	afx_msg void OnBnClickedWebSend();
};
